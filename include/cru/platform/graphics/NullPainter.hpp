#pragma once
#include "Painter.hpp"

namespace cru::platform::graphics {
class NullPainter : public Object, public virtual IPainter {
 public:
  NullPainter() = default;

  CRU_DELETE_COPY(NullPainter)
  CRU_DELETE_MOVE(NullPainter)

  ~NullPainter() override = default;

 public:
  String GetPlatformId() const override { return u"NULL"; }

  String GetDebugString() override { return u"NullPainter"; }

  Matrix GetTransform() override { return Matrix(); }
  void SetTransform(const Matrix& matrix) override {}

  void ConcatTransform(const Matrix& matrix) override {}

  void Clear(const Color& color) override {}

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override{};
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override {}
  void FillRectangle(const Rect& rectangle, IBrush* brush) override {}
  void StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                     float width) override {}
  void FillEllipse(const Rect& outline_rect, IBrush* brush,
                   float width) override {}

  void StrokeGeometry(IGeometry* geometry, IBrush* brush,
                      float width) override {}
  void FillGeometry(IGeometry* geometry, IBrush* brush) override {}

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override {}

  void PushLayer(const Rect& bounds) override{};

  void PopLayer() override {}

  void PushState() override {}

  void PopState() override {}

  void EndDraw() override {}
};
}  // namespace cru::platform::graphics
