#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasPainter.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"

#include <optional>

namespace cru::platform::graphics::web_canvas {

namespace {
void contextSetTransform(emscripten::val context, const Matrix& matrix) {
  context.call<void>("setTransform", matrix.m11, matrix.m12, matrix.m21,
                     matrix.m22, matrix.m31, matrix.m32);
}
}  // namespace

WebCanvasPainter::WebCanvasPainter(WebCanvasGraphicsFactory* factory,
                                   emscripten::val context,
                                   std::optional<Matrix> current_transform)
    : WebCanvasResource(factory),
      context_(context),
      current_transform_(current_transform.value_or(Matrix::Identity())) {
  contextSetTransform(context_, current_transform_);
}

WebCanvasPainter::~WebCanvasPainter() {}

Matrix WebCanvasPainter::GetTransform() { return current_transform_; }

void WebCanvasPainter::SetTransform(const Matrix& transform) {
  current_transform_ = transform;
  contextSetTransform(context_, transform);
}
}  // namespace cru::platform::graphics::web_canvas
