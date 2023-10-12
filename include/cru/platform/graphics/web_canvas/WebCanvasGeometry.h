#pragma once

#include "../Geometry.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>

namespace cru::platform::graphics::web_canvas {
/**
 * \remarks See IGeometryBuilder for platform limitation.
 */
class WebCanvasGeometryBuilder : public WebCanvasResource,
                                 public virtual IGeometryBuilder {
 public:
  WebCanvasGeometryBuilder(WebCanvasGraphicsFactory* factory);

  ~WebCanvasGeometryBuilder() override;

  Point GetCurrentPosition() override;

  void MoveTo(const Point& point) override;

 private:
  Point current_postion_;
  emscripten::val path2d_;
};
}  // namespace cru::platform::graphics::web_canvas
