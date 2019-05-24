#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

namespace cru::platform::graph {
struct IBrush : public virtual Interface {};

struct ISolidColorBrush : public virtual IBrush {
  virtual ui::Color GetColor() = 0;
  virtual void SetColor(const ui::Color& color) = 0;
};
}  // namespace cru::platform
