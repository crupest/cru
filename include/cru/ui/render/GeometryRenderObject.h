#pragma once
#include "RenderObject.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <optional>

namespace cru::ui::render {
class GeometryRenderObject : public RenderObject {
 public:
  GeometryRenderObject();

  CRU_DELETE_COPY(GeometryRenderObject)
  CRU_DELETE_MOVE(GeometryRenderObject)

  ~GeometryRenderObject() override;

 public:
  void Draw(platform::graphics::IPainter* painter) override;
  RenderObject* HitTest(const Point& point) override;

  std::shared_ptr<platform::graphics::IGeometry> GetGeometry() const;
  void SetGeometry(std::shared_ptr<platform::graphics::IGeometry> geometry,
                   std::optional<Rect> view_port = std::nullopt);

  Rect GetViewPort() const;
  void SetViewPort(const Rect& view_port);

  std::shared_ptr<platform::graphics::IBrush> GetFillBrush() const;
  void SetFillBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  std::shared_ptr<platform::graphics::IBrush> GetStrokeBrush() const;
  void SetStrokeBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  float GetStrokeWidth() const;
  void SetStrokeWidth(float width);

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  Size OnMeasureContent1(const BoxConstraint& constraint) override;

 private:
  std::shared_ptr<platform::graphics::IGeometry> geometry_ = nullptr;
  Rect view_port_{};
  std::shared_ptr<platform::graphics::IBrush> fill_brush_ = nullptr;
  std::shared_ptr<platform::graphics::IBrush> stroke_brush_ = nullptr;
  float stroke_width_ = 0;
};
}  // namespace cru::ui::render
