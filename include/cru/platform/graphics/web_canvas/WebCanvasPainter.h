#pragma once
#include "../Painter.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>

namespace cru::platform::graphics::web_canvas {
class WebCanvasPainter: public WebCanvasResource, public virtual IPainter {
  public:
    WebCanvasPainter(WebCanvasGraphicsFactory* factory);
};
}
