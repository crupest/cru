#pragma once
#include "../Base.h"

#include <optional>

namespace cru::ui::style {
struct ApplyBorderStyleInfo {
  ApplyBorderStyleInfo() = default;

  explicit ApplyBorderStyleInfo(
      std::optional<std::shared_ptr<platform::graphics::IBrush>> border_brush,
      std::optional<Thickness> border_thickness = std::nullopt,
      std::optional<CornerRadius> border_radius = std::nullopt,
      std::optional<std::shared_ptr<platform::graphics::IBrush>>
          foreground_brush = std::nullopt,
      std::optional<std::shared_ptr<platform::graphics::IBrush>>
          background_brush = std::nullopt)
      : border_brush(std::move(border_brush)),
        border_thickness(std::move(border_thickness)),
        border_radius(std::move(border_radius)),
        foreground_brush(std::move(foreground_brush)),
        background_brush(std::move(background_brush)) {}

  std::optional<std::shared_ptr<platform::graphics::IBrush>> border_brush;
  std::optional<Thickness> border_thickness;
  std::optional<CornerRadius> border_radius;
  std::optional<std::shared_ptr<platform::graphics::IBrush>> foreground_brush;
  std::optional<std::shared_ptr<platform::graphics::IBrush>> background_brush;
};
}  // namespace cru::ui::style
