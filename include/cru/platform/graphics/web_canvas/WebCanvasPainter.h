#pragma once
#include "../Painter.h"
#include "WebCanvasRef.h"
#include "WebCanvasResource.h"

#include <emscripten/val.h>
#include <optional>

namespace cru::platform::graphics::web_canvas {
/**
 * Notes about Transform:
 * Although there is a Matrix object now supported by browsers, it is not
 * generally available in old browsers. However, now that we are using
 * WebAssembly, the browsers should be quite up to date, but there is still a
 * window between the two things. And it is common for a platform to not support
 * getting transform and only allow you to do transformation.
 */
class WebCanvasPainter : public WebCanvasResource, public virtual IPainter {
 public:
  WebCanvasPainter(WebCanvasGraphicsFactory* factory, emscripten::val context,
                   std::optional<Matrix> init_transform = std::nullopt);

  ~WebCanvasPainter() override;

 public:
  Matrix GetTransform() override;
  void SetTransform(const Matrix& transform) override;

  void ConcatTransform(const Matrix& matrix) override;

  void Clear(const Color& color) override;

  emscripten::val GetCanvas2DContext() const { return context_; }
  WebCanvasRef GetCanvas();

 private:
  Matrix current_transform_;
  emscripten::val context_;
};
}  // namespace cru::platform::graphics::web_canvas
