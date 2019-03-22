#pragma once
#include "pre.hpp"

#include <wrl/client.h>  // for ComPtr

#include "render_object.hpp"

// forward declarations
struct ID2D1Brush;
struct ID2D1Geometry;

struct CornerRadius {
  float left_top;
  float right_top;
  float left_bottom;
  float right_bottom;
};

namespace cru::ui::render {
class BorderRenderObject : public RenderObject {
 public:
  explicit BorderRenderObject(Microsoft::WRL::ComPtr<ID2D1Brush> brush);
  BorderRenderObject(const BorderRenderObject& other) = delete;
  BorderRenderObject(BorderRenderObject&& other) = delete;
  BorderRenderObject& operator=(const BorderRenderObject& other) = delete;
  BorderRenderObject& operator=(BorderRenderObject&& other) = delete;
  ~BorderRenderObject() override = default;

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool enabled) { is_enabled_ = enabled; }

  Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const { return border_brush_; }
  void SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush> new_brush) {
    border_brush_ = std::move(new_brush);
  }

  Thickness GetBorderWidth() const { return border_width_; }
  void SetBorderWidth(const Thickness& thickness) { border_width_ = thickness; }

  CornerRadius GetCornerRadius() const { return corner_radius_; }
  void SetCornerRadius(const CornerRadius& new_corner_radius) {
    corner_radius_ = new_corner_radius;
  }

  void RecreateGeometry();  // TODO

  void Draw(ID2D1RenderTarget* render_target) override;  // TODO

  RenderObject* HitTest(const Point& point) override;  // TODO

 protected:
  void OnMeasureCore(const Size& available_size) override;
  void OnLayoutCore(const Rect& rect) override;
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

 private:
  bool is_enabled_;

  Microsoft::WRL::ComPtr<ID2D1Brush> border_brush_;
  Thickness border_width_;
  CornerRadius corner_radius_;

  Microsoft::WRL::ComPtr<ID2D1Geometry> geometry_;
};
}  // namespace cru::ui::render
