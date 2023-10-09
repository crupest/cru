#include "cru/platform/graphics/web_canvas/WebCanvasBrush.h"
#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"

namespace cru::platform::graphics::web_canvas {
WebCanvasBrush::WebCanvasBrush(WebCanvasGraphicsFactory* factory)
    : WebCanvasResource(factory) {}

WebCanvasBrush::~WebCanvasBrush() {}

WebCanvasSolidColorBrush::WebCanvasSolidColorBrush(
    WebCanvasGraphicsFactory* factory, const Color& color)
    : WebCanvasBrush(factory), color_(color) {}

Color WebCanvasSolidColorBrush::GetColor() { return color_; }

void WebCanvasSolidColorBrush::SetColor(const Color& color) { color_ = color; }

String WebCanvasSolidColorBrush::GetCssColor() const {
  return color_.ToString();
}

emscripten::val WebCanvasSolidColorBrush::GetStyle() {
  return emscripten::val::u16string(color_.ToString().c_str());
}

}  // namespace cru::platform::graphics::web_canvas
