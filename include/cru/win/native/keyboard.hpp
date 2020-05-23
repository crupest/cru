#pragma once
#include "Base.hpp"

#include "cru/platform/native/Keyboard.hpp"

namespace cru::platform::native::win {
KeyCode VirtualKeyToKeyCode(int virtual_key);
KeyModifier RetrieveKeyMofifier();
}  // namespace cru::platform::native::win
