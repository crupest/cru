#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"

namespace cru::platform::graphics::web_canvas {
WebCanvasGraphicsFactory::WebCanvasGraphicsFactory()
    : WebCanvasResource(this) {}

WebCanvasGraphicsFactory::~WebCanvasGraphicsFactory() {}
}  // namespace cru::platform::graphics::web_canvas
