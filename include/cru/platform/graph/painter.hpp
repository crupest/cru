#pragma once
#include "cru/common/base.hpp"

#include "../matrix.hpp"
#include "cru/common/endable.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform::graph {
struct IBrush;
struct IGeometry;
struct ITextLayout;

struct IPainter : virtual Interface, virtual IEndable<void> {
  virtual Matrix GetTransform() = 0;
  virtual void SetTransform(const Matrix& matrix) = 0;
  virtual void Clear(const ui::Color& color) = 0;
  virtual void StrokeRectangle(const ui::Rect& rectangle, IBrush* brush,
                               float width) = 0;
  virtual void FillRectangle(const ui::Rect& rectangle, IBrush* brush) = 0;
  virtual void StrokeGeometry(IGeometry* geometry, IBrush* brush,
                              float width) = 0;
  virtual void FillGeometry(IGeometry* geometry, IBrush* brush) = 0;
  virtual void DrawText(const ui::Point& offset, ITextLayout* text_layout,
                        IBrush* brush) = 0;
};
}  // namespace cru::platform::graph
