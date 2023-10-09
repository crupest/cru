#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasPainter.h"
#include "cru/platform/graphics/web_canvas/WebCanvasRef.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"

#include <optional>

namespace cru::platform::graphics::web_canvas {

namespace {
void contextDoTransform(emscripten::val context, const char* method,
                        const Matrix& matrix) {
  context.call<void>(method, matrix.m11, matrix.m12, matrix.m21, matrix.m22,
                     matrix.m31, matrix.m32);
}
}  // namespace

WebCanvasPainter::WebCanvasPainter(WebCanvasGraphicsFactory* factory,
                                   emscripten::val context,
                                   std::optional<Matrix> current_transform)
    : WebCanvasResource(factory),
      context_(context),
      current_transform_(current_transform.value_or(Matrix::Identity())) {
  contextDoTransform(context_, "setTransform", current_transform_);
}

WebCanvasPainter::~WebCanvasPainter() {}

Matrix WebCanvasPainter::GetTransform() { return current_transform_; }

void WebCanvasPainter::SetTransform(const Matrix& transform) {
  current_transform_ = transform;
  contextDoTransform(context_, "setTransform", current_transform_);
}

void WebCanvasPainter::ConcatTransform(const Matrix& transform) {
  current_transform_ *= transform;
  contextDoTransform(context_, "transform", transform);
}

void WebCanvasPainter::Clear(const Color& color) {}

WebCanvasRef WebCanvasPainter::GetCanvas() {
  return WebCanvasRef(context_["canvas"]);
}
}  // namespace cru::platform::graphics::web_canvas
