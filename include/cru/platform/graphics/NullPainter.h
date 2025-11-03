#pragma once
#include "Painter.h"

#include <cru/base/Base.h>

namespace cru::platform::graphics {
class NullPainter : public Object, public virtual IPainter {
 public:
  NullPainter() = default;

  CRU_DELETE_COPY(NullPainter)
  CRU_DELETE_MOVE(NullPainter)

  ~NullPainter() override = default;

 public:
  std::string GetPlatformId() const override { return "NULL"; }

  std::string GetDebugString() override { return "NullPainter"; }

  Matrix GetTransform() override { return Matrix(); }
  void SetTransform(const Matrix& matrix) override { CRU_UNUSED(matrix) }

  void ConcatTransform(const Matrix& matrix) override { CRU_UNUSED(matrix) }

  void Clear(const Color& color) override { CRU_UNUSED(color) }

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override {
    CRU_UNUSED(start) CRU_UNUSED(end) CRU_UNUSED(brush) CRU_UNUSED(width)
  }
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override {
    CRU_UNUSED(rectangle) CRU_UNUSED(brush) CRU_UNUSED(width)
  }
  void FillRectangle(const Rect& rectangle, IBrush* brush) override {
    CRU_UNUSED(rectangle)
    CRU_UNUSED(brush)
  }
  void StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                     float width) override {
    CRU_UNUSED(outline_rect)
    CRU_UNUSED(brush)
    CRU_UNUSED(width)
  }
  void FillEllipse(const Rect& outline_rect, IBrush* brush) override {
    CRU_UNUSED(outline_rect)
    CRU_UNUSED(brush)
  }

  void StrokeGeometry(IGeometry* geometry, IBrush* brush,
                      float width) override {
    CRU_UNUSED(geometry)
    CRU_UNUSED(brush)
    CRU_UNUSED(width)
  }
  void FillGeometry(IGeometry* geometry, IBrush* brush) override {
    CRU_UNUSED(geometry)
    CRU_UNUSED(brush)
  }

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override {
    CRU_UNUSED(offset)
    CRU_UNUSED(text_layout)
    CRU_UNUSED(brush)
  }

  void DrawImage(const Point& offset, IImage* image) override {
    CRU_UNUSED(offset)
    CRU_UNUSED(image)
  }

  void PushLayer(const Rect& bounds) override { CRU_UNUSED(bounds) }

  void PopLayer() override {}

  void PushState() override {}

  void PopState() override {}

  void EndDraw() override {}
};
}  // namespace cru::platform::graphics
