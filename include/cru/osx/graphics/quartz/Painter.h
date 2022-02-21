#pragma once
#include "Resource.h"
#include "cru/common/Base.h"
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/Painter.h"

#include <CoreGraphics/CoreGraphics.h>

#include <functional>

namespace cru::platform::graphics::osx::quartz {
class QuartzCGContextPainter : public OsxQuartzResource,
                               public virtual IPainter {
  CRU_DEFINE_CLASS_LOG_TAG(u"QuartzCGContextPainter")

 public:
  explicit QuartzCGContextPainter(
      IGraphicsFactory* graphics_factory, CGContextRef cg_context,
      bool auto_release, const Size& size,
      std::function<void(QuartzCGContextPainter*)> on_end_draw);

  CRU_DELETE_COPY(QuartzCGContextPainter)
  CRU_DELETE_MOVE(QuartzCGContextPainter)

  ~QuartzCGContextPainter() override;

 public:
  Matrix GetTransform() override;
  void SetTransform(const Matrix& matrix) override;

  void ConcatTransform(const Matrix& matrix) override;

  void Clear(const Color& color) override;

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override;
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, IBrush* brush) override;
  void StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                     float width) override;
  void FillEllipse(const Rect& outline_rect, IBrush* brush) override;

  void StrokeGeometry(IGeometry* geometry, IBrush* brush, float width) override;
  void FillGeometry(IGeometry* geometry, IBrush* brush) override;

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override;

  void DrawImage(const Point& offset, IImage* image) override;

  void PushLayer(const Rect& bounds) override;

  void PopLayer() override;

  void EndDraw() override;

  void PushState() override;

  void PopState() override;

 private:
  void SetLineWidth(float width);

  void DoEndDraw();

  void Validate();

 private:
  CGContextRef cg_context_;

  bool auto_release_;

  Size size_;

  Matrix transform_;

  std::function<void(QuartzCGContextPainter*)> on_end_draw_;

  std::vector<Rect> clip_stack_;
};
}  // namespace cru::platform::graphics::osx::quartz
