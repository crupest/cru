#pragma once

#include <cru/platform/gui/Keyboard.h>

#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon.h>
#include <unordered_map>

namespace cru::platform::gui::xcb {
struct XcbUiApplication;

KeyCode XorgKeysymToCruKeyCode(xcb_keysym_t keysym);
std::vector<xcb_keysym_t> XorgKeycodeToKeysyms(XcbUiApplication* application,
                                               xcb_keycode_t keycode);
KeyCode XorgKeycodeToCruKeyCode(XcbUiApplication* application,
                                xcb_keycode_t keycode);
std::unordered_map<KeyCode, bool> GetKeyboardState(
    XcbUiApplication* application);
KeyModifier GetCurrentKeyModifiers(XcbUiApplication* application);

/**
 * Used to convert state field of xcb_button_press_event_t,
 * xcb_button_release_event_t, xcb_key_press_event_t, xcb_key_release_event_t.
 */
KeyModifier ConvertModifiersOfEvent(uint32_t mask);

class XcbKeyboardManager {
 public:
  explicit XcbKeyboardManager(XcbUiApplication* application);
  ~XcbKeyboardManager();

  std::string KeysymToUtf8(xcb_keysym_t keysym);
  std::string KeycodeToUtf8(xcb_keycode_t keycode);

 private:
  XcbUiApplication* application_;
  xkb_context* xkb_context_;
  xkb_keymap* xkb_keymap_;
  xkb_state* xkb_state_;
};
}  // namespace cru::platform::gui::xcb
