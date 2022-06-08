#pragma once
#include "../Painter.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class CairoPainter : public CairoResource, public virtual IPainter {
 public:
  CairoPainter(CairoGraphicsFactory* factory, cairo_t* cairo,
               bool auto_release);
  ~CairoPainter();

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

  void PushState() override;

  void PopState() override;

  void EndDraw() override;

 private:
  cairo_t* cairo_;
  bool auto_release_;
};
}  // namespace cru::platform::graphics::cairo
