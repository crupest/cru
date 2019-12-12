#pragma once
#include "resource.hpp"

namespace cru::platform::graph {
struct IBrush : virtual IGraphResource {};

struct ISolidColorBrush : virtual IBrush {
  virtual Color GetColor() = 0;
  virtual void SetColor(const Color& color) = 0;
};
}  // namespace cru::platform::graph
