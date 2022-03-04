#pragma once
#include "../Base.h"
#include "cru/platform/graphics/Font.h"

namespace cru::ui::controls {
struct CRU_UI_API IFontControl : virtual Interface {
  virtual std::shared_ptr<platform::graphics::IFont> GetFont() const = 0;
  virtual void SetFont(std::shared_ptr<platform::graphics::IFont> font) = 0;
};
}  // namespace cru::ui::controls
