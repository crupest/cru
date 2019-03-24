#pragma once
#include "pre.hpp"

#include <wrl/client.h>  // for ComPtr

#include "render_object.hpp"

// forward declarations
struct ID2D1Brush;
struct ID2D1Geometry;

namespace cru::ui::render {
struct CornerRadius {
  constexpr CornerRadius()
      : left_top(), right_top(), left_bottom(), right_bottom() {}
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

class BorderRenderObject : public RenderObject {
 public:
  explicit BorderRenderObject(ID2D1Brush* brush);
  BorderRenderObject(const BorderRenderObject& other) = delete;
  BorderRenderObject(BorderRenderObject&& other) = delete;
  BorderRenderObject& operator=(const BorderRenderObject& other) = delete;
  BorderRenderObject& operator=(BorderRenderObject&& other) = delete;
  ~BorderRenderObject() override;

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool enabled) { is_enabled_ = enabled; }

  ID2D1Brush* GetBrush() const { return border_brush_; }
  void SetBrush(ID2D1Brush* new_brush);

  Thickness GetBorderWidth() const { return border_thickness_; }
  void SetBorderWidth(const Thickness& thickness) { border_thickness_ = thickness; }

  CornerRadius GetCornerRadius() const { return corner_radius_; }
  void SetCornerRadius(const CornerRadius& new_corner_radius) {
    corner_radius_ = new_corner_radius;
  }

  void RecreateGeometry();

  void Draw(ID2D1RenderTarget* render_target) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, int position) override;

  void OnMeasureCore(const Size& available_size) override;
  void OnLayoutCore(const Rect& rect) override;
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

 private:
  bool is_enabled_ = false;

  ID2D1Brush* border_brush_ = nullptr;
  Thickness border_thickness_ = Thickness::Zero();
  CornerRadius corner_radius_{};

  ID2D1Geometry* geometry_ = nullptr;
  ID2D1Geometry* border_outer_geometry_ = nullptr;
};
}  // namespace cru::ui::render
