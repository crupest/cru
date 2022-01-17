#pragma once
#include <string_view>
#include "../style/ApplyBorderStyleInfo.hpp"
#include "RenderObject.hpp"
#include "cru/platform/GraphicsBase.hpp"
#include "cru/ui/Base.hpp"

namespace cru::ui::render {
class CRU_UI_API BorderRenderObject : public RenderObject {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::render::BorderRenderObject")

 public:
  BorderRenderObject();
  BorderRenderObject(const BorderRenderObject& other) = delete;
  BorderRenderObject(BorderRenderObject&& other) = delete;
  BorderRenderObject& operator=(const BorderRenderObject& other) = delete;
  BorderRenderObject& operator=(BorderRenderObject&& other) = delete;
  ~BorderRenderObject() override;

  bool IsBorderEnabled() const { return is_border_enabled_; }
  void SetBorderEnabled(bool enabled) { is_border_enabled_ = enabled; }

  std::shared_ptr<platform::graphics::IBrush> GetBorderBrush() {
    return border_brush_;
  }

  void SetBorderBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    if (brush == border_brush_) return;
    border_brush_ = std::move(brush);
    InvalidatePaint();
  }

  Thickness GetBorderThickness() const { return border_thickness_; }

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

  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() {
    return foreground_brush_;
  }

  void SetForegroundBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    if (brush == foreground_brush_) return;
    foreground_brush_ = std::move(brush);
    InvalidatePaint();
  }

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() {
    return background_brush_;
  }

  void SetBackgroundBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    if (brush == background_brush_) return;
    background_brush_ = std::move(brush);
    InvalidatePaint();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style);

  RenderObject* HitTest(const Point& point) override;

  Thickness GetOuterSpaceThickness() const override;
  Rect GetPaddingRect() const override;
  Rect GetContentRect() const override;

  std::u16string_view GetName() const override { return u"BorderRenderObject"; }

 protected:
  void OnDrawCore(platform::graphics::IPainter* painter) override;

  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  void OnAfterLayout() override;

 private:
  void RecreateGeometry();

 private:
  bool is_border_enabled_ = false;

  std::shared_ptr<platform::graphics::IBrush> border_brush_;
  Thickness border_thickness_;
  CornerRadius border_radius_;

  std::shared_ptr<platform::graphics::IBrush> foreground_brush_;
  std::shared_ptr<platform::graphics::IBrush> background_brush_;

  // The ring. Used for painting.
  std::unique_ptr<platform::graphics::IGeometry> geometry_;
  // Area including inner area of the border. Used for painting foreground and
  // background.
  std::unique_ptr<platform::graphics::IGeometry> border_inner_geometry_;
  // Area including border ring and inner area. Used for hit test.
  std::unique_ptr<platform::graphics::IGeometry> border_outer_geometry_;
};
}  // namespace cru::ui::render
