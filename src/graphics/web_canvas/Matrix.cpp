#include "cru/graphics/web_canvas/WebCanvasMatrix.h"
#include "cru/web/Js.h"

namespace cru::graphics::web_canvas {
emscripten::val CreateDomMatrix(const Matrix &matrix) {
  return web::js::Construct(
      "DOMMatrix", std::vector<float>{matrix.m11, matrix.m12, matrix.m21,
                                      matrix.m22, matrix.m31, matrix.m32});
}
}  // namespace cru::graphics::web_canvas
