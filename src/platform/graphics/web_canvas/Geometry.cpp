#include <memory>
#include "cru/platform/graphics/web_canvas/WebCanvasGeometry.h"
#include "cru/platform/graphics/web_canvas/WebCanvasGraphicsFactory.h"
#include "cru/platform/graphics/web_canvas/WebCanvasMatrix.h"
#include "cru/platform/graphics/web_canvas/WebCanvasResource.h"
#include "cru/platform/web/Js.h"

namespace cru::platform::graphics::web_canvas {
WebCanvasGeometry::WebCanvasGeometry(WebCanvasGraphicsFactory* factory,
                                     emscripten::val canvas,
                                     emscripten::val path2d)
    : WebCanvasResource(factory),
      canvas_(std::move(canvas)),
      path2d_(std::move(path2d)) {}

WebCanvasGeometry::~WebCanvasGeometry() {}

std::unique_ptr<IGeometry> WebCanvasGeometry::Transform(const Matrix& matrix) {
  auto new_path = web::js::Construct("Path2D");
  auto js_matrix = CreateDomMatrix(matrix);
  new_path.call<void>("addPath", js_matrix);
  return std::make_unique<WebCanvasGeometry>(GetFactory(), canvas_,
                                             std::move(new_path));
}

WebCanvasGeometryBuilder::WebCanvasGeometryBuilder(
    WebCanvasGraphicsFactory* factory, emscripten::val canvas)
    : WebCanvasResource(factory), canvas_(std::move(canvas)) {}

WebCanvasGeometryBuilder::~WebCanvasGeometryBuilder() {}

std::unique_ptr<IGeometry> WebCanvasGeometryBuilder::Build() {
  auto new_path = web::js::Construct("Path2D", GetPathData());
  return std::make_unique<WebCanvasGeometry>(GetFactory(), canvas_,
                                             std::move(new_path));
}
}  // namespace cru::platform::graphics::web_canvas
