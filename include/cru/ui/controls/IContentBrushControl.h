#pragma once
#include "../Base.h"
#include "cru/platform/graphics/Brush.h"

namespace cru::ui::controls {
struct CRU_UI_API IContentBrushControl : virtual Interface {
  virtual std::shared_ptr<platform::graphics::IBrush> GetContentBrush()
      const = 0;
  virtual void SetContentBrush(
      std::shared_ptr<platform::graphics::IBrush> brush) = 0;
};
}  // namespace cru::ui::controls
