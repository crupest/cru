#pragma once
#include "Base.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IBrush : virtual IGraphicsResource {};

struct CRU_PLATFORM_GRAPHICS_API ISolidColorBrush : virtual IBrush {
  virtual Color GetColor() = 0;
  virtual void SetColor(const Color& color) = 0;
};
}  // namespace cru::platform::graphics
