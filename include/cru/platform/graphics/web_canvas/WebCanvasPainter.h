#pragma once
#include "../Painter.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>
#include <optional>

namespace cru::platform::graphics::web_canvas {
class WebCanvasPainter : public WebCanvasResource, public virtual IPainter {
 public:
  WebCanvasPainter(WebCanvasGraphicsFactory* factory, emscripten::val context,
                   std::optional<Matrix> init_transform = std::nullopt);

  ~WebCanvasPainter() override;

 public:
  Matrix GetTransform() override;
  void SetTransform(const Matrix& transform) override;

 private:
  Matrix current_transform_;
  emscripten::val context_;
};
}  // namespace cru::platform::graphics::web_canvas
