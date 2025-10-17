#pragma once
#include "Base.h"

namespace cru::platform::gui {
struct IClipboard : virtual IPlatformResource {
  virtual std::string GetText() = 0;
  virtual void SetText(std::string text) = 0;
};
}  // namespace cru::platform::gui
