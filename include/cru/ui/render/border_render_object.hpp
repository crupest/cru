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
  std::shared_ptr<platform::graph::Brush> brush{};
  Thickness thickness{};
  CornerRadius corner_radius{};
};

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

  BorderStyle* GetBorderStyle() {
    return &border_style_;
  }

  std::shared_ptr<platform::graph::Brush> GetForegroundBrush() {
    return foreground_brush_;
  }

  void SetForegroundBrush(std::shared_ptr<platform::graph::Brush> brush) {
    foreground_brush_ = std::move(brush);
  }

  std::shared_ptr<platform::graph::Brush> GetBackgroundBrush() {
    return foreground_brush_;
  }

  void SetBackgroundBrush(std::shared_ptr<platform::graph::Brush> brush) {
    foreground_brush_ = std::move(brush);
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
  bool is_border_enabled_ = false;
  BorderStyle border_style_;

  std::shared_ptr<platform::graph::Brush> foreground_brush_;
  std::shared_ptr<platform::graph::Brush> background_brush_;

  // The ring. Used for painting.
  std::unique_ptr<platform::graph::Geometry> geometry_;
  // Area including inner area of the border. Used for painting foreground and background.
  std::unique_ptr<platform::graph::Geometry> border_inner_geometry_;
  // Area including border ring and inner area. Used for hit test.
  std::unique_ptr<platform::graph::Geometry> border_outer_geometry_;
};
}  // namespace cru::ui::render
