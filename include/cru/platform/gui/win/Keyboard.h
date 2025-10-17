#pragma once
#include "Base.h"

#include <cru/platform/gui/Input.h>

namespace cru::platform::gui::win {
KeyCode CRU_WIN_GUI_API VirtualKeyToKeyCode(int virtual_key);
KeyModifier CRU_WIN_GUI_API RetrieveKeyMofifier();
}  // namespace cru::platform::gui::win
