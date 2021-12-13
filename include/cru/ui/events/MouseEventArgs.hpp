#pragma once
#include "UiEventArgs.hpp"

#include <optional>

namespace cru::ui::events {
class MouseEventArgs : public UiEventArgs {
 public:
  MouseEventArgs(Object* sender, Object* original_sender,
                 const std::optional<Point>& point = std::nullopt)
      : UiEventArgs(sender, original_sender), point_(point) {}
  MouseEventArgs(const MouseEventArgs& other) = default;
  MouseEventArgs(MouseEventArgs&& other) = default;
  MouseEventArgs& operator=(const MouseEventArgs& other) = default;
  MouseEventArgs& operator=(MouseEventArgs&& other) = default;
  ~MouseEventArgs() override = default;

  // This point is relative to window client lefttop.
  Point GetPoint() const { return point_.value_or(Point{}); }
  Point GetPoint(render::RenderObject* render_target) const;
  Point GetPointToContent(render::RenderObject* render_target) const;

 private:
  std::optional<Point> point_;
};
}  // namespace cru::ui::event
