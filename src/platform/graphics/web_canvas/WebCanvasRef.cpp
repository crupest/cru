#include "cru/platform/graphics/web_canvas/WebCanvasRef.h"
#include "cru/platform/web/Js.h"

#include <cassert>
#include <utility>

namespace cru::platform::graphics::web_canvas {
WebCanvasRef::WebCanvasRef(emscripten::val canvas_val)
    : val_(std::move(canvas_val)) {
  assert(web::js::IsNotNullAndInstanceOf(val_, "HTMLCanvasElement"));
}

int WebCanvasRef::GetWidth() const { return val_["width"].as<int>(); }

int WebCanvasRef::GetHeight() const { return val_["height"].as<int>(); }
}  // namespace cru::platform::graphics::web_canvas
