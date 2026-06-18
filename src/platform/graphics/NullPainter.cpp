#include "cru/platform/graphics/NullPainter.h"

namespace cru::platform::graphics {
std::string NullPainter::GetPlatformId() const { return "NULL"; }

std::string NullPainter::GetDebugString() { return "NullPainter"; }

Matrix NullPainter::GetTransform() { return Matrix(); }

void NullPainter::SetTransform(const Matrix& matrix) { CRU_UNUSED(matrix) }

void NullPainter::ConcatTransform(const Matrix& matrix) { CRU_UNUSED(matrix) }

void NullPainter::Clear(const Color& color) { CRU_UNUSED(color) }

void NullPainter::DrawLine(const Point& start, const Point& end, IBrush* brush,
                           float width) {
  CRU_UNUSED(start)
  CRU_UNUSED(end)
  CRU_UNUSED(brush)
  CRU_UNUSED(width)
}

void NullPainter::StrokeRectangle(const Rect& rectangle, IBrush* brush,
                                  float width) {
  CRU_UNUSED(rectangle)
  CRU_UNUSED(brush)
  CRU_UNUSED(width)
}

void NullPainter::FillRectangle(const Rect& rectangle, IBrush* brush) {
  CRU_UNUSED(rectangle)
  CRU_UNUSED(brush)
}

void NullPainter::StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                                float width) {
  CRU_UNUSED(outline_rect)
  CRU_UNUSED(brush)
  CRU_UNUSED(width)
}

void NullPainter::FillEllipse(const Rect& outline_rect, IBrush* brush) {
  CRU_UNUSED(outline_rect)
  CRU_UNUSED(brush)
}

void NullPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                 float width) {
  CRU_UNUSED(geometry)
  CRU_UNUSED(brush)
  CRU_UNUSED(width)
}

void NullPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  CRU_UNUSED(geometry)
  CRU_UNUSED(brush)
}

void NullPainter::DrawText(const Point& offset, ITextLayout* text_layout,
                           IBrush* brush) {
  CRU_UNUSED(offset)
  CRU_UNUSED(text_layout)
  CRU_UNUSED(brush)
}

void NullPainter::DrawImage(const Point& offset, IImage* image) {
  CRU_UNUSED(offset)
  CRU_UNUSED(image)
}

void NullPainter::PushLayer(const Rect& bounds) { CRU_UNUSED(bounds) }

void NullPainter::PopLayer() {}

void NullPainter::PushState() {}

void NullPainter::PopState() {}

void NullPainter::EndDraw() {}

}  // namespace cru::platform::graphics
