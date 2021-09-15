#pragma once
#include "Resource.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/graphics/Painter.hpp"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
class QuartzCGContextPainter : public OsxQuartzResource,
                               public virtual IPainter {
 public:
  explicit QuartzCGContextPainter(IGraphicsFactory* graphics_factory,
                                  CGContextRef cg_context)
      : OsxQuartzResource(graphics_factory), cg_context_(cg_context) {}

  CRU_DELETE_COPY(QuartzCGContextPainter)
  CRU_DELETE_MOVE(QuartzCGContextPainter)

  ~QuartzCGContextPainter() override = default;

 public:
  Matrix GetTransform() override;
  void SetTransform(const Matrix& matrix) override;

  void Clear(const Color& color) override;

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override;
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, IBrush* brush) override;

  void StrokeGeometry(IGeometry* geometry, IBrush* brush, float width) override;
  void FillGeometry(IGeometry* geometry, IBrush* brush) override;

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override;

  void PushLayer(const Rect& bounds) override;

  void PopLayer() override;

  void EndDraw() override;

 private:
  CGContextRef cg_context_;
};
}  // namespace cru::platform::graphics::osx::quartz
