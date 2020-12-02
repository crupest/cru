#pragma once
#include "../Base.hpp"

namespace cru::ui::style {
struct ApplyBorderStyleInfo {
  std::shared_ptr<platform::graphics::IBrush> border_brush;
  std::optional<Thickness> border_thickness;
  std::optional<CornerRadius> border_radius;
  std::shared_ptr<platform::graphics::IBrush> foreground_brush;
  std::shared_ptr<platform::graphics::IBrush> background_brush;
};
}  // namespace cru::ui::style
