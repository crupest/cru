#include "cru/platform/graphics/web_canvas/WebCanvasGeometry.h"
#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"
#include "cru/platform/web/Js.h"

namespace cru::platform::graphics::web_canvas {
WebCanvasGeometryBuilder::WebCanvasGeometryBuilder(
    WebCanvasGraphicsFactory* factory)
    : WebCanvasResource(factory) {
  path2d_ = web::js::Construct("Path2D");
}
}  // namespace cru::platform::graphics::web_canvas
