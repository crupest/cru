#pragma once
#include "RenderObject.hpp"

namespace cru::ui::render {
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

  std::shared_ptr<platform::graph::IBrush> GetBorderBrush() {
    return border_brush_;
  }

  void SetBorderBrush(std::shared_ptr<platform::graph::IBrush> brush) {
    if (brush == border_brush_) return;
    border_brush_ = std::move(brush);
    InvalidatePaint();
  }

  Thickness GetBorderThickness() { return border_thickness_; }

  void SetBorderThickness(const Thickness thickness) {
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

  std::shared_ptr<platform::graph::IBrush> GetForegroundBrush() {
    return foreground_brush_;
  }

  void SetForegroundBrush(std::shared_ptr<platform::graph::IBrush> brush) {
    if (brush == foreground_brush_) return;
    foreground_brush_ = std::move(brush);
    InvalidatePaint();
  }

  std::shared_ptr<platform::graph::IBrush> GetBackgroundBrush() {
    return background_brush_;
  }

  void SetBackgroundBrush(std::shared_ptr<platform::graph::IBrush> brush) {
    if (brush == background_brush_) return;
    background_brush_ = std::move(brush);
    InvalidatePaint();
  }

  void SetBorderStyle(const BorderStyle& style);

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  Size OnMeasureCore(const MeasureRequirement& requirement) override;
  void OnLayoutCore(const Size& size) override;
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;

  void OnAfterLayout() override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

  void RecreateGeometry();

 private:
  bool is_border_enabled_ = false;

  std::shared_ptr<platform::graph::IBrush> border_brush_;
  Thickness border_thickness_;
  CornerRadius border_radius_;

  std::shared_ptr<platform::graph::IBrush> foreground_brush_;
  std::shared_ptr<platform::graph::IBrush> background_brush_;

  // The ring. Used for painting.
  std::unique_ptr<platform::graph::IGeometry> geometry_;
  // Area including inner area of the border. Used for painting foreground and
  // background.
  std::unique_ptr<platform::graph::IGeometry> border_inner_geometry_;
  // Area including border ring and inner area. Used for hit test.
  std::unique_ptr<platform::graph::IGeometry> border_outer_geometry_;
};
}  // namespace cru::ui::render
