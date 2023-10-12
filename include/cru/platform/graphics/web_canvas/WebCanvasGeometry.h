#pragma once

#include "../Geometry.h"
#include "../SvgGeometryBuilderMixin.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>
#include <memory>

namespace cru::platform::graphics::web_canvas {
class WebCanvasGeometry : public WebCanvasResource, public virtual IGeometry {
 public:
  WebCanvasGeometry(WebCanvasGraphicsFactory* factory, emscripten::val path2d);
  ~WebCanvasGeometry() override;

  bool StrokeContains(float width, const Point& point) override;
  virtual bool FillContains(const Point& point) = 0;
  virtual Rect GetBounds() = 0;
  virtual std::unique_ptr<IGeometry> Transform(const Matrix& matrix) = 0;
 private:
  emscripten::val path2d_;
};

/**
 * \remarks See IGeometryBuilder for platform limitation.
 */
class WebCanvasGeometryBuilder : public WebCanvasResource,
                                 public SvgGeometryBuilderMixin {
 public:
  WebCanvasGeometryBuilder(WebCanvasGraphicsFactory* factory);

  ~WebCanvasGeometryBuilder() override;

  std::unique_ptr<IGeometry> Build() override;
};
}  // namespace cru::platform::graphics::web_canvas
