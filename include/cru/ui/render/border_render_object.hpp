#pragma once
#include "cru/platform/graph/brush.hpp"
#include "cru/ui/base.hpp"
#include "render_object.hpp"

#include <memory>

namespace cru::platform::graph {
class Brush;
class Geometry;
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

struct BorderStyle {
  std::shared_ptr<platform::graph::Brush> brush;
  Thickness thickness;
  CornerRadius corner_radius;
};

class BorderRenderObject : public RenderObject {
 public:
  explicit BorderRenderObject(std::shared_ptr<platform::graph::Brush> brush);
  BorderRenderObject(const BorderRenderObject& other) = delete;
  BorderRenderObject(BorderRenderObject&& other) = delete;
  BorderRenderObject& operator=(const BorderRenderObject& other) = delete;
  BorderRenderObject& operator=(BorderRenderObject&& other) = delete;
  ~BorderRenderObject() override = default;

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool enabled) { is_enabled_ = enabled; }

  std::shared_ptr<platform::graph::Brush> GetBrush() const {
    return style_.brush;
  }
  void SetBrush(std::shared_ptr<platform::graph::Brush> new_brush) {
    style_.brush = std::move(new_brush);
  }

  Thickness GetBorderWidth() const { return style_.thickness; }
  // Remember to call Refresh after set shape properties.
  void SetBorderWidth(const Thickness& thickness) {
    style_.thickness = thickness;
  }

  CornerRadius GetCornerRadius() const { return style_.corner_radius; }
  // Remember to call Refresh after set shape properties.
  void SetCornerRadius(const CornerRadius& new_corner_radius) {
    style_.corner_radius = new_corner_radius;
  }

  BorderStyle GetStyle() const {
    return style_;
  }
  // Remember to call Refresh after set shape properties.
  void SetStyle(BorderStyle newStyle) {
    style_ = std::move(newStyle);
  }

  void Refresh() { RecreateGeometry(); }

  void Draw(platform::graph::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, int position) override;

  void OnSizeChanged(const Size& old_size, const Size& new_size) override;

  void OnMeasureCore(const Size& available_size) override;
  void OnLayoutCore(const Rect& rect) override;
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

  void RecreateGeometry();

 private:
  bool is_enabled_ = false;

  BorderStyle style_;

  // The ring. Used for painting.
  std::shared_ptr<platform::graph::Geometry> geometry_ = nullptr;
  // Area including border ring and inner area. Used for hit test.
  std::shared_ptr<platform::graph::Geometry> border_outer_geometry_ = nullptr;
};
}  // namespace cru::ui::render
