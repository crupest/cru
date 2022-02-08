#pragma once
#include "cru/platform/gui/Keyboard.h"

namespace cru::platform::gui::osx {
KeyCode KeyCodeFromOsxToCru(unsigned short n);
unsigned short KeyCodeFromCruToOsx(KeyCode k);
}  // namespace cru::platform::gui::osx
