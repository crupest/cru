#pragma once
#include "Resource.hpp"

namespace cru::platform::graphics {
struct IBrush : virtual IGraphicsResource {};

struct ISolidColorBrush : virtual IBrush {
  virtual Color GetColor() = 0;
  virtual void SetColor(const Color& color) = 0;
};
}  // namespace cru::platform::graphics
