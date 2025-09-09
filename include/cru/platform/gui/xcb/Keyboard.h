#pragma once

#include <cru/platform/gui/Keyboard.h>

namespace cru::platform::gui::xcb {
struct XcbUiApplication;
KeyModifier GetCurrentKeyModifiers(XcbUiApplication* application);
}  // namespace cru::platform::gui::xcb
