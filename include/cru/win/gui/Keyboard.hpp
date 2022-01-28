#pragma once
#include "Base.hpp"

#include "cru/platform/gui/Keyboard.hpp"

namespace cru::platform::gui::win {
KeyCode CRU_WIN_GUI_API VirtualKeyToKeyCode(int virtual_key);
KeyModifier CRU_WIN_GUI_API RetrieveKeyMofifier();
}  // namespace cru::platform::gui::win
