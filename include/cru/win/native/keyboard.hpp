#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/native/keyboard.hpp"

namespace cru::platform::native::win {
KeyCode VirtualKeyToKeyCode(int virtual_key);
KeyModifier RetrieveKeyMofifier();
}  // namespace cru::platform::native::win
