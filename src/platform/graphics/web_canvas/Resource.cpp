#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"

namespace cru::platform::graphics::web_canvas {
const String WebCanvasResource::kPlatformId = u"WebCanvas";

WebCanvasResource::WebCanvasResource(WebCanvasGraphicsFactory* factory)
    : factory_(factory) {}

WebCanvasResource::~WebCanvasResource() {}

String WebCanvasResource::GetPlatformId() const { return kPlatformId; }

}  // namespace cru::platform::graphics::web_canvas
