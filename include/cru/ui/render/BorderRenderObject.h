#pragma once
#include "SingleChildRenderObject.h"

#include "../style/ApplyBorderStyleInfo.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
class CRU_UI_API BorderRenderObject : public SingleChildRenderObject {
  CRU_DEFINE_CLASS_LOG_TAG("BorderRenderObject")

 public:
  BorderRenderObject();
  CRU_DELETE_COPY(BorderRenderObject)
  CRU_DELETE_MOVE(BorderRenderObject)
  ~BorderRenderObject() override;

  bool IsBorderEnabled() const { return is_border_enabled_; }
  void SetBorderEnabled(bool enabled);

  std::shared_ptr<platform::graphics::IBrush> GetBorderBrush() {
    return border_brush_;
  }
  void SetBorderBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  Thickness GetBorderThickness() const { return border_thickness_; }
  void SetBorderThickness(const Thickness thickness);

  CornerRadius GetBorderRadius() { return border_radius_; }
  void SetBorderRadius(const CornerRadius radius);

  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() {
    return foreground_brush_;
  }

  void SetForegroundBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() {
    return background_brush_;
  }

  void SetBackgroundBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style);

  RenderObject* HitTest(const Point& point) override;
  void Draw(platform::graphics::IPainter* painter) override;

  Thickness GetTotalSpaceThickness() const override;
  Thickness GetInnerSpaceThickness() const override;
  Rect GetPaddingRect() const override;
  Rect GetContentRect() const override;

  std::string GetName() const override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  Size OnMeasureContent1(const BoxConstraint& constraint) override;

  void OnResize(const Size& new_size) override;

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
