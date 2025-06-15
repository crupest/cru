#include "cru/graphics/web_canvas/WebCanvasResource.h"

namespace cru::graphics::web_canvas {
const String WebCanvasResource::kPlatformId = u"WebCanvas";

WebCanvasResource::WebCanvasResource(WebCanvasGraphicsFactory* factory)
    : factory_(factory) {}

WebCanvasResource::~WebCanvasResource() {}

String WebCanvasResource::GetPlatformId() const { return kPlatformId; }

}  // namespace cru::graphics::web_canvas
