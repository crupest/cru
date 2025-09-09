#pragma once

#include <cru/platform/gui/Keyboard.h>

#include <xcb/xcb.h>
#include <unordered_map>

namespace cru::platform::gui::xcb {
struct XcbUiApplication;

KeyCode XorgKeysymToKeyCode(xcb_keysym_t keysym);
KeyCode XorgKeycodeToCruKeyCode(XcbUiApplication* application, xcb_keycode_t keycode);
std::unordered_map<KeyCode, bool> GetKeyboardState(XcbUiApplication* application);
KeyModifier GetCurrentKeyModifiers(XcbUiApplication* application);
}  // namespace cru::platform::gui::xcb
