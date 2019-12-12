#pragma once
#include "cru/platform/graph/brush.hpp"
#include "cru/ui/base.hpp"
#include "render_object.hpp"

#include <memory>

namespace cru::platform::graph {
class Brush;
class IGeometry;
}  // namespace cru::platform::graph

namespace cru::ui::render {
struct CornerRadius {
  constexpr CornerRadius()
      : left_top(), right_top(), left_bottom(), right_bottom() {}
  constexpr CornerRadius(const Point& value)
      : left_top(value),
        right_top(value),
        left_bottom(value),
        right_bottom(value) {}
  constexpr CornerRadius(Point left_top, Point right_top, Point left_bottom,
                         Point right_bottom)
      : left_top(left_top),
        right_top(right_top),
        left_bottom(left_bottom),
        right_bottom(right_bottom) {}

  Point left_top;
  Point right_top;
  Point left_bottom;
  Point right_bottom;
};

inline bool operator==(const CornerRadius& left, const CornerRadius& right) {
  return left.left_top == right.left_top &&
         left.left_bottom == right.left_bottom &&
         left.right_top == right.right_top &&
         left.right_bottom == right.right_bottom;
}

inline bool operator!=(const CornerRadius& left, const CornerRadius& right) {
  return !(left == right);
}

class BorderRenderObject : public RenderObject {
 public:
  BorderRenderObject();
  BorderRenderObject(const BorderRenderObject& other) = delete;
  BorderRenderObject(BorderRenderObject&& other) = delete;
  BorderRenderObject& operator=(const BorderRenderObject& other) = delete;
  BorderRenderObject& operator=(BorderRenderObject&& other) = delete;
  ~BorderRenderObject() override;

  bool IsBorderEnabled() const { return is_border_enabled_; }
  void SetBorderEnabled(bool enabled) { is_border_enabled_ = enabled; }

  std::shared_ptr<platform::graph::Brush> GetBorderBrush() {
    return border_brush_;
  }

  void SetBorderBrush(std::shared_ptr<platform::graph::Brush> brush) {
    if (brush == border_brush_) return;
    border_brush_ = std::move(brush);
    InvalidatePaint();
  }

  platform::Thickness GetBorderThickness() { return border_thickness_; }

  void SetBorderThickness(const platform::Thickness thickness) {
    if (thickness == border_thickness_) return;
    border_thickness_ = thickness;
    InvalidateLayout();
  }

  CornerRadius GetBorderRadius() { return border_radius_; }

  void SetBorderRadius(const CornerRadius radius) {
    if (radius == border_radius_) return;
    border_radius_ = radius;
    RecreateGeometry();
  }

  std::shared_ptr<platform::graph::Brush> GetForegroundBrush() {
    return foreground_brush_;
  }

  void SetForegroundBrush(std::shared_ptr<platform::graph::Brush> brush) {
    if (brush == foreground_brush_) return;
    foreground_brush_ = std::move(brush);
    InvalidatePaint();
  }

  std::shared_ptr<platform::graph::Brush> GetBackgroundBrush() {
    return background_brush_;
  }

  void SetBackgroundBrush(std::shared_ptr<platform::graph::Brush> brush) {
    if (brush == background_brush_) return;
    background_brush_ = std::move(brush);
    InvalidatePaint();
  }

  void Draw(platform::graph::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnMeasureCore(const Size& available_size) override;
  void OnLayoutCore(const Rect& rect) override;
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  void OnAfterLayout() override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

  void RecreateGeometry();

 private:
  bool is_border_enabled_ = false;

  std::shared_ptr<platform::graph::Brush> border_brush_;
  platform::Thickness border_thickness_;
  CornerRadius border_radius_;

  std::shared_ptr<platform::graph::Brush> foreground_brush_;
  std::shared_ptr<platform::graph::Brush> background_brush_;

  // The ring. Used for painting.
  std::unique_ptr<platform::graph::IGeometry> geometry_;
  // Area including inner area of the border. Used for painting foreground and
  // background.
  std::unique_ptr<platform::graph::IGeometry> border_inner_geometry_;
  // Area including border ring and inner area. Used for hit test.
  std::unique_ptr<platform::graph::IGeometry> border_outer_geometry_;
};
}  // namespace cru::ui::render
