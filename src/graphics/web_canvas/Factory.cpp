#include "cru/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/graphics/web_canvas/WebCanvasResource.h"

namespace cru::graphics::web_canvas {
WebCanvasGraphicsFactory::WebCanvasGraphicsFactory()
    : WebCanvasResource(this) {}

WebCanvasGraphicsFactory::~WebCanvasGraphicsFactory() {}
}  // namespace cru::graphics::web_canvas
