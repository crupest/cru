#pragma once
#include "Base.hpp"

namespace cru::platform::gui {
struct IClipboard : virtual IPlatformResource {
  virtual String GetText() = 0;
  virtual void SetText(String text) = 0;
};
}  // namespace cru::platform::gui
