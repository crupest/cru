#pragma once
#include "Base.hpp"

#include "cru/platform/gui/Keyboard.hpp"

namespace cru::platform::gui::win {
KeyCode VirtualKeyToKeyCode(int virtual_key);
KeyModifier RetrieveKeyMofifier();
}  // namespace cru::platform::gui::win
