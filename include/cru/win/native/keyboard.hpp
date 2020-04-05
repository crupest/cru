#pragma once
#include "base.hpp"

#include "cru/platform/native/keyboard.hpp"

namespace cru::platform::native::win {
KeyCode VirtualKeyToKeyCode(int virtual_key);
KeyModifier RetrieveKeyMofifier();
}  // namespace cru::platform::native::win
