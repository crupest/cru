#include "cru/platform/gui/win/Keyboard.h"

namespace cru::platform::gui::win {
KeyCode VirtualKeyToKeyCode(int virtual_key) {
  if (virtual_key >= 0x30 && virtual_key <= 0x39) {
    return KeyCode{static_cast<int>(KeyCode::N0) + (virtual_key - 0x30)};
  } else if (virtual_key >= 0x41 && virtual_key <= 0x5a) {
    return KeyCode{static_cast<int>(KeyCode::A) + (virtual_key - 0x41)};
  } else if (virtual_key >= VK_NUMPAD0 && virtual_key <= VK_NUMPAD9) {
    return KeyCode{static_cast<int>(KeyCode::NumPad0) +
                   (virtual_key - VK_NUMPAD0)};
  } else if (virtual_key >= VK_F1 && virtual_key <= VK_F12) {
    return KeyCode{static_cast<int>(KeyCode::F1) + (virtual_key - VK_F1)};
  } else {
    switch (virtual_key) {
#define CRU_MAP_KEY(virtual_key, keycode) \
  case virtual_key:                       \
    return KeyCode::keycode;

      CRU_MAP_KEY(VK_LBUTTON, LeftButton)
      CRU_MAP_KEY(VK_MBUTTON, MiddleButton)
      CRU_MAP_KEY(VK_RBUTTON, RightButton)
      CRU_MAP_KEY(VK_ESCAPE, Escape)
      CRU_MAP_KEY(VK_OEM_3, GraveAccent)
      CRU_MAP_KEY(VK_TAB, Tab)
      CRU_MAP_KEY(VK_CAPITAL, CapsLock)
      CRU_MAP_KEY(VK_LSHIFT, LeftShift)
      CRU_MAP_KEY(VK_LCONTROL, LeftCtrl)
      CRU_MAP_KEY(VK_LWIN, LeftSuper)
      CRU_MAP_KEY(VK_LMENU, LeftAlt)
      CRU_MAP_KEY(VK_OEM_MINUS, Minus)
      CRU_MAP_KEY(VK_OEM_PLUS, Equal)
      CRU_MAP_KEY(VK_BACK, Backspace)
      CRU_MAP_KEY(VK_OEM_4, LeftSquareBracket)
      CRU_MAP_KEY(VK_OEM_6, RightSquareBracket)
      CRU_MAP_KEY(VK_OEM_5, BackSlash)
      CRU_MAP_KEY(VK_OEM_1, Semicolon)
      CRU_MAP_KEY(VK_OEM_7, Quote)
      CRU_MAP_KEY(VK_OEM_COMMA, Comma)
      CRU_MAP_KEY(VK_OEM_PERIOD, Period)
      CRU_MAP_KEY(VK_OEM_2, Slash)
      CRU_MAP_KEY(VK_RSHIFT, RightShift)
      CRU_MAP_KEY(VK_RCONTROL, RightCtrl)
      CRU_MAP_KEY(VK_RWIN, RightSuper)
      CRU_MAP_KEY(VK_RMENU, RightAlt)
      CRU_MAP_KEY(VK_INSERT, Insert)
      CRU_MAP_KEY(VK_DELETE, Delete)
      CRU_MAP_KEY(VK_HOME, Home)
      CRU_MAP_KEY(VK_END, End)
      CRU_MAP_KEY(VK_PRIOR, PageUp)
      CRU_MAP_KEY(VK_NEXT, PageDown)
      CRU_MAP_KEY(VK_UP, Up)
      CRU_MAP_KEY(VK_LEFT, Left)
      CRU_MAP_KEY(VK_DOWN, Down)
      CRU_MAP_KEY(VK_RIGHT, Right)
      CRU_MAP_KEY(VK_SNAPSHOT, PrintScreen)
      CRU_MAP_KEY(VK_PAUSE, Pause)

#undef CRU_MAP_KEY

      default:
        return KeyCode::Unknown;
    }
  }
}

KeyModifier RetrieveKeyMofifier() {
  KeyModifier result{0};
  if (::GetKeyState(VK_SHIFT) < 0) result |= KeyModifiers::shift;
  if (::GetKeyState(VK_CONTROL) < 0) result |= KeyModifiers::ctrl;
  if (::GetKeyState(VK_MENU) < 0) result |= KeyModifiers::alt;
  return result;
}
}  // namespace cru::platform::gui::win
