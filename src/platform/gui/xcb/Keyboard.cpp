#include "cru/platform/gui/xcb/Keyboard.h"
#include "cru/base/Guard.h"
#include "cru/base/StringUtil.h"
#include "cru/platform/gui/Keyboard.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <bitset>
#include <cctype>
#include <climits>
#include <unordered_map>
#include <utility>

namespace cru::platform::gui::xcb {
// Refer to
// https://www.x.org/releases/X11R7.7/doc/xproto/x11protocol.html#keysym_encoding
KeyCode XorgKeysymToKeyCode(xcb_keysym_t keysym) {
  if (keysym >= 'A' && keysym <= 'Z') {
    return KeyCode(static_cast<int>(KeyCode::A) + (keysym - 'A'));
  }

  if (keysym >= 'a' && keysym <= 'z') {
    return KeyCode(static_cast<int>(KeyCode::A) + (keysym - 'a'));
  }

  if (keysym >= '0' && keysym <= '9') {
    return KeyCode(static_cast<int>(KeyCode::N0) + (keysym - '0'));
  }

  if (keysym >= 0xFFB0 && keysym <= 0xFFB9) {
    return KeyCode(static_cast<int>(KeyCode::NumPad0) + (keysym - 0xFFB0));
  }

  if (keysym >= 0xFFBE && keysym <= 0xFFC9) {
    return KeyCode(static_cast<int>(KeyCode::F1) + (keysym - 0xFFBE));
  }

  switch (keysym) {
#define CRU_DEFINE_KEYCODE_MAP(keysym, cru) \
  case keysym:                              \
    return cru;

    CRU_DEFINE_KEYCODE_MAP(',', KeyCode::Comma)
    CRU_DEFINE_KEYCODE_MAP('.', KeyCode::Period)
    CRU_DEFINE_KEYCODE_MAP('/', KeyCode::Slash)
    CRU_DEFINE_KEYCODE_MAP(';', KeyCode::Semicolon)
    CRU_DEFINE_KEYCODE_MAP('\'', KeyCode::Quote)
    CRU_DEFINE_KEYCODE_MAP('{', KeyCode::LeftSquareBracket)
    CRU_DEFINE_KEYCODE_MAP('}', KeyCode::RightSquareBracket)
    CRU_DEFINE_KEYCODE_MAP('-', KeyCode::Minus)
    CRU_DEFINE_KEYCODE_MAP('=', KeyCode::Equal)
    CRU_DEFINE_KEYCODE_MAP('\\', KeyCode::BackSlash)
    CRU_DEFINE_KEYCODE_MAP(0xFF1B, KeyCode::Escape)
    CRU_DEFINE_KEYCODE_MAP(0xFF09, KeyCode::Tab)
    CRU_DEFINE_KEYCODE_MAP(0xFFE5, KeyCode::CapsLock)
    CRU_DEFINE_KEYCODE_MAP(0xFFE1, KeyCode::LeftShift)
    CRU_DEFINE_KEYCODE_MAP(0xFFE2, KeyCode::RightShift)
    CRU_DEFINE_KEYCODE_MAP(0xFFE3, KeyCode::LeftCtrl)
    CRU_DEFINE_KEYCODE_MAP(0xFFE4, KeyCode::RightCtrl)
    CRU_DEFINE_KEYCODE_MAP(0xFFE9, KeyCode::LeftAlt)
    CRU_DEFINE_KEYCODE_MAP(0xFFEA, KeyCode::RightAlt)
    CRU_DEFINE_KEYCODE_MAP(0xFF08, KeyCode::Backspace)
    CRU_DEFINE_KEYCODE_MAP(0xFF0D, KeyCode::Return)
    CRU_DEFINE_KEYCODE_MAP(0xFFFF, KeyCode::Delete)
    CRU_DEFINE_KEYCODE_MAP(0xFF50, KeyCode::Home)
    CRU_DEFINE_KEYCODE_MAP(0xFF57, KeyCode::End)
    CRU_DEFINE_KEYCODE_MAP(0xFF55, KeyCode::PageUp)
    CRU_DEFINE_KEYCODE_MAP(0xFF56, KeyCode::PageDown)
    CRU_DEFINE_KEYCODE_MAP(0xFF51, KeyCode::Left)
    CRU_DEFINE_KEYCODE_MAP(0xFF53, KeyCode::Right)
    CRU_DEFINE_KEYCODE_MAP(0xFF52, KeyCode::Up)
    CRU_DEFINE_KEYCODE_MAP(0xFF54, KeyCode::Down)
    CRU_DEFINE_KEYCODE_MAP(' ', KeyCode::Space)
    default:
      return KeyCode::Unknown;
  }
}

std::vector<xcb_keysym_t> XorgKeycodeToKeysyms(XcbUiApplication *application,
                                               xcb_keycode_t keycode) {
  auto connection = application->GetXcbConnection();
  auto setup = xcb_get_setup(connection);
  auto min_keycode = setup->min_keycode;
  auto max_keycode = setup->max_keycode;

  // Get keyboard mapping
  auto mapping_cookie = xcb_get_keyboard_mapping(connection, keycode, 1);
  auto mapping_reply = FreeLater(
      xcb_get_keyboard_mapping_reply(connection, mapping_cookie, NULL));

  if (!mapping_reply) {
    throw XcbException("Cannot get keyboard mapping.");
  }

  auto keysyms_per_keycode = mapping_reply->keysyms_per_keycode;
  auto *keysyms = xcb_get_keyboard_mapping_keysyms(mapping_reply);

  std::vector<xcb_keysym_t> result;
  for (int i = 0; i < keysyms_per_keycode; i++) {
    result.push_back(keysyms[i]);
  }
  return result;
}

KeyCode XorgKeycodeToCruKeyCode(XcbUiApplication *application,
                                xcb_keycode_t keycode) {
  auto keysyms = XorgKeycodeToKeysyms(application, keycode);

  for (auto keysym : keysyms) {
    auto result = XorgKeysymToKeyCode(keysym);
    if (result != KeyCode::Unknown) return result;
  }

  return KeyCode::Unknown;
}

std::string XorgKeysymToUtf8(xcb_keysym_t keysym, bool upper) {
  if (0x20 <= keysym && keysym <= 0x7e || 0xa0 <= keysym && keysym <= 0xff) {
    return std::string{
        static_cast<char>(upper ? std::toupper(keysym) : keysym)};
  }

  if (0x01000100 <= keysym && keysym <= 0x0110FFFF) {
    auto code_point = keysym - 0x01000000;
    std::string result;
    Utf8EncodeCodePointAppend(keysym,
                              [&result](char c) { result.push_back(c); });
    return result;
  }

  return {};
}

namespace {
using KeymapBitset =
    std::bitset<sizeof(std::declval<xcb_query_keymap_reply_t>().keys) *
                CHAR_BIT>;

KeymapBitset GetXorgKeymap(xcb_connection_t *connection) {
  auto keymap_cookie = xcb_query_keymap(connection);
  auto keymap_reply =
      FreeLater(xcb_query_keymap_reply(connection, keymap_cookie, NULL));

  if (!keymap_reply) {
    throw XcbException("Cannot get keymap.");
  }

  KeymapBitset result;
  int counter = 0;
  for (auto member : keymap_reply->keys) {
    for (int i = 0; i < sizeof(member); i++) {
      result[counter] = member & (1 << i);
      counter++;
    }
  }

  return result;
}
}  // namespace

std::unordered_map<KeyCode, bool> GetKeyboardState(
    XcbUiApplication *application) {
  auto connection = application->GetXcbConnection();
  auto setup = xcb_get_setup(connection);
  auto min_keycode = setup->min_keycode;
  auto max_keycode = setup->max_keycode;

  // Get keyboard mapping
  auto mapping_cookie = xcb_get_keyboard_mapping(connection, min_keycode,
                                                 max_keycode - min_keycode + 1);
  auto mapping_reply = FreeLater(
      xcb_get_keyboard_mapping_reply(connection, mapping_cookie, NULL));

  if (!mapping_reply) {
    throw XcbException("Cannot get keyboard mapping.");
  }

  auto keysyms_per_keycode = mapping_reply->keysyms_per_keycode;
  auto *keysyms = xcb_get_keyboard_mapping_keysyms(mapping_reply);

  auto keymap = GetXorgKeymap(connection);

  std::unordered_map<KeyCode, bool> result;

  for (xcb_keycode_t i = min_keycode; i <= max_keycode; i++) {
    auto keysyms_for_this = keysyms + (i - min_keycode) * keysyms_per_keycode;
    for (int j = 0; j < keysyms_per_keycode; j++) {
      auto keycode = XorgKeysymToKeyCode(keysyms_for_this[j]);
      if (keycode != KeyCode::Unknown) {
        result[keycode] = keymap[i];
      }
    }
  }

  return result;
}

// Though X provides GetModifierMapping, it cannot get ALT state.
KeyModifier GetCurrentKeyModifiers(XcbUiApplication *application) {
  KeyModifier result{};
  auto state = GetKeyboardState(application);
  if (state[KeyCode::LeftShift] || state[KeyCode::RightShift]) {
    result |= KeyModifiers::Shift;
  }
  if (state[KeyCode::LeftCtrl] || state[KeyCode::RightCtrl]) {
    result |= KeyModifiers::Ctrl;
  }
  if (state[KeyCode::LeftAlt] || state[KeyCode::RightAlt]) {
    result |= KeyModifiers::Alt;
  }
  return result;
}
}  // namespace cru::platform::gui::xcb
