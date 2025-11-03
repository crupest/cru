#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/web_canvas/WebCanvasBrush.h"
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

void WebCanvasPainter::Clear(const Color& color) {
  auto canvas = GetCanvas();
  auto canvas_width = canvas.GetWidth();
  auto canvas_height = canvas.GetHeight();

  context_.set("fillStyle", color.ToString());
  context_.call<void>("fillRect", 0, 0, canvas_width, canvas_height);
}

void WebCanvasPainter::DrawLine(const Point& start, const Point& end,
                                IBrush* brush, float width) {
  SetStrokeStyle(brush, width);
  context_.call<void>("beginPath");
  context_.call<void>("moveTo", start.x, start.y);
  context_.call<void>("lineTo", end.x, end.y);
  context_.call<void>("stroke");
}

void WebCanvasPainter::StrokeRectangle(const Rect& rectangle, IBrush* brush,
                                       float width) {
  SetStrokeStyle(brush, width);
  context_.call<void>("strokeRect", rectangle.left, rectangle.top,
                      rectangle.width, rectangle.height);
}

void WebCanvasPainter::FillRectangle(const Rect& rectangle, IBrush* brush) {
  SetFillStyle(brush);
  context_.call<void>("fillRect", rectangle.left, rectangle.top,
                      rectangle.width, rectangle.height);
}

void WebCanvasPainter::StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                                     float width) {
  SetStrokeStyle(brush);
  // TODO: Need to use path.
}
void WebCanvasPainter::FillEllipse(const Rect& outline_rect, IBrush* brush) {
  SetFillStyle(brush);
  // TODO: Need to use path.
}

void WebCanvasPainter::SetStrokeStyle(IBrush* brush, float width) {
  context_.set("strokeStyle", ConvertBrush(brush)->GetStyle());
  if (width > 0) {
    context_.set("lineWidth", width);
  }
}

void WebCanvasPainter::SetFillStyle(IBrush* brush) {
  context_.set("fillStyle", ConvertBrush(brush)->GetStyle());
}

WebCanvasBrush* WebCanvasPainter::ConvertBrush(IBrush* brush) const {
  return CheckPlatform<WebCanvasBrush>(brush, GetPlatformId());
}

WebCanvasRef WebCanvasPainter::GetCanvas() {
  return WebCanvasRef(context_["canvas"]);
}
}  // namespace cru::platform::graphics::web_canvas
