#pragma once

#include "../Geometry.h"
#include "../SvgGeometryBuilderMixin.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>
#include <memory>

namespace cru::platform::graphics::web_canvas {
class WebCanvasGeometry : public WebCanvasResource, public virtual IGeometry {
 public:
  WebCanvasGeometry(WebCanvasGraphicsFactory* factory, emscripten::val canvas,
                    emscripten::val path2d);
  ~WebCanvasGeometry() override;

  bool StrokeContains(float width, const Point& point) override;

  bool FillContains(const Point& point) override;

  Rect GetBounds() override;

  std::unique_ptr<IGeometry> Transform(const Matrix& matrix) override;

 private:
  emscripten::val canvas_;
  emscripten::val path2d_;
};

/**
 * \remarks See IGeometryBuilder for platform limitation.
 */
class WebCanvasGeometryBuilder : public WebCanvasResource,
                                 public SvgGeometryBuilderMixin {
 public:
  WebCanvasGeometryBuilder(WebCanvasGraphicsFactory* factory,
                           emscripten::val canvas);

  ~WebCanvasGeometryBuilder() override;

  std::unique_ptr<IGeometry> Build() override;

 private:
  emscripten::val canvas_;
};
}  // namespace cru::platform::graphics::web_canvas
