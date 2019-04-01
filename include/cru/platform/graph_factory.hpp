#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

namespace cru::platform {
struct SolidColorBrush;
struct GeometryBuilder;

struct GraphFactory : virtual Interface {
  virtual SolidColorBrush* CreateSolidColorBrush(const ui::Color& color) = 0;
  virtual GeometryBuilder* CreateGeometryBuilder() = 0;
};
}  // namespace cru::platform
