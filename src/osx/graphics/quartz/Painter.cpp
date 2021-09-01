#include "cru/osx/graphics/quartz/Painter.hpp"

#include "cru/osx/graphics/quartz/Brush.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Geometry.hpp"
#include "cru/platform/Check.hpp"

namespace cru::platform::graphics::osx::quartz {
Matrix QuartzCGContextPainter::GetTransform() {
  return Convert(CGContextGetCTM(cg_context_));
}

void QuartzCGContextPainter::SetTransform(const Matrix& matrix) {
  auto old = CGContextGetCTM(cg_context_);
  old = CGAffineTransformInvert(old);
  CGContextConcatCTM(cg_context_, old);
  CGContextConcatCTM(cg_context_, Convert(matrix));
}

void QuartzCGContextPainter::Clear(const Color& color) {
  // TODO:
}

void QuartzCGContextPainter::DrawLine(const Point& start, const Point& end,
                                      IBrush* brush, float width) {
  CGContextBeginPath(cg_context_);
  CGContextMoveToPoint(cg_context_, start.x, start.y);
  CGContextAddLineToPoint(cg_context_, end.x, end.y);

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextSetLineWidth(cg_context_, width);
  CGContextStrokePath(cg_context_);
}

void QuartzCGContextPainter::StrokeRectangle(const Rect& rectangle,
                                             IBrush* brush, float width) {
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextStrokeRectWithWidth(cg_context_, Convert(rectangle), width);
}

void QuartzCGContextPainter::FillRectangle(const Rect& rectangle,
                                           IBrush* brush) {
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextFillRect(cg_context_, Convert(rectangle));
}

void QuartzCGContextPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                            float width) {
  QuartzGeometry* g = CheckPlatform<QuartzGeometry>(geometry, GetPlatformId());
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());

  b->Select(cg_context_);
  CGContextSetLineWidth(cg_context_, width);
  CGContextBeginPath(cg_context_);
  CGContextAddPath(cg_context_, g->GetCGPath());
  CGContextStrokePath(cg_context_);
}

void QuartzCGContextPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  QuartzGeometry* g = CheckPlatform<QuartzGeometry>(geometry, GetPlatformId());
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());

  b->Select(cg_context_);
  CGContextBeginPath(cg_context_);
  CGContextAddPath(cg_context_, g->GetCGPath());
  CGContextFillPath(cg_context_);
}
}  // namespace cru::platform::graphics::osx::quartz
