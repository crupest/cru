#pragma once
#include "Base.h"

#include <cru/platform/gui/Clipboard.h>

namespace cru::platform::gui::sdl {
class SdlClipboard : public SdlResource, public virtual IClipboard {
 public:
  std::string GetText() override;
  void SetText(std::string text) override;
};
}  // namespace cru::platform::gui::sdl
