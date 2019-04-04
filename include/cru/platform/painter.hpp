#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"
#include "matrix.hpp"

namespace cru::platform {
struct Brush;
struct Geometry;
struct TextLayout;

struct Painter : virtual Interface {
  virtual Matrix GetTransform() = 0;
  virtual void SetTransform(const Matrix& matrix) = 0;
  virtual void StrokeRectangle(const ui::Rect& rectangle, Brush* brush,
                               float width) = 0;
  virtual void FillRectangle(const ui::Rect& rectangle, Brush* brush) = 0;
  virtual void StrokeGeometry(Geometry* geometry, Brush* brush,
                              float width) = 0;
  virtual void FillGeometry(Geometry* geometry, Brush* brush) = 0;
  virtual void DrawText(const ui::Point& offset, TextLayout* text_layout, Brush* brush);
  virtual void EndDraw() = 0;
  virtual bool IsDisposed() = 0;
};
}  // namespace cru::platform
