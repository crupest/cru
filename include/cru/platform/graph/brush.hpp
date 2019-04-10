#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

namespace cru::platform::graph {
struct Brush : public virtual Interface {};

struct SolidColorBrush : public virtual Brush {
  virtual ui::Color GetColor() = 0;
  virtual void SetColor(const ui::Color& color) = 0;
};
}  // namespace cru::platform
