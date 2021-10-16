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
                                  CGContextRef cg_context, bool auto_release,
                                  const Size& size)
      : OsxQuartzResource(graphics_factory),
        cg_context_(cg_context),
        auto_release_(auto_release),
        size_(size) {}

  CRU_DELETE_COPY(QuartzCGContextPainter)
  CRU_DELETE_MOVE(QuartzCGContextPainter)

  ~QuartzCGContextPainter() override;

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
  void Validate();

 private:
  CGContextRef cg_context_;

  bool auto_release_;

  Size size_;
};
}  // namespace cru::platform::graphics::osx::quartz
