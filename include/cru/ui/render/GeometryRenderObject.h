#pragma once
#include "MeasureRequirement.h"
#include "RenderObject.h"

#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Geometry.h>

#include <optional>

namespace cru::ui::render {
class GeometryRenderObject : public RenderObject {
 public:
  static constexpr auto kRenderObjectName = "GeometryRenderObject";

  GeometryRenderObject();

 public:
  void Draw(platform::graphics::IPainter* painter) override;
  RenderObject* HitTest(const Point& point) override;

  std::shared_ptr<platform::graphics::IGeometry> GetGeometry();
  void SetGeometry(std::shared_ptr<platform::graphics::IGeometry> geometry,
                   std::optional<Rect> view_port = std::nullopt);

  Rect GetViewPort();
  void SetViewPort(const Rect& view_port);

  std::shared_ptr<platform::graphics::IBrush> GetFillBrush();
  void SetFillBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  std::shared_ptr<platform::graphics::IBrush> GetStrokeBrush();
  void SetStrokeBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  float GetStrokeWidth();
  void SetStrokeWidth(float width);

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  std::shared_ptr<platform::graphics::IGeometry> geometry_ = nullptr;
  Rect view_port_{};
  std::shared_ptr<platform::graphics::IBrush> fill_brush_ = nullptr;
  std::shared_ptr<platform::graphics::IBrush> stroke_brush_ = nullptr;
  float stroke_width_ = 0;
};
}  // namespace cru::ui::render
