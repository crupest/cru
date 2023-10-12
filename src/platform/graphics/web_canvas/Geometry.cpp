#include "cru/platform/graphics/web_canvas/WebCanvasGeometry.h"
#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"
#include "cru/platform/web/Js.h"

namespace cru::platform::graphics::web_canvas {
bool WebCanvasGeometry::StrokeContains(float width, const Point& point) {}

WebCanvasGeometryBuilder::WebCanvasGeometryBuilder(
    WebCanvasGraphicsFactory* factory)
    : WebCanvasResource(factory) {}

WebCanvasGeometryBuilder::~WebCanvasGeometryBuilder() {}
}  // namespace cru::platform::graphics::web_canvas
