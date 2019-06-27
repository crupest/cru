#pragma once
#include "../graphic_base.hpp"
#include "../matrix.hpp"
#include "../native_resource.hpp"

namespace cru::platform::graph {
class Brush;
class Geometry;
class TextLayout;

class Painter : public NativeResource {
 protected:
  Painter() = default;

 public:
  Painter(const Painter& other) = delete;
  Painter& operator=(const Painter& other) = delete;

  Painter(Painter&& other) = delete;
  Painter& operator=(Painter&& other) = delete;

  ~Painter() override = default;

 public:
  virtual Matrix GetTransform() = 0;
  virtual void SetTransform(const Matrix& matrix) = 0;

  virtual void Clear(const Color& color) = 0;

  virtual void StrokeRectangle(const Rect& rectangle, Brush* brush,
                               float width) = 0;
  virtual void FillRectangle(const Rect& rectangle, Brush* brush) = 0;

  virtual void StrokeGeometry(Geometry* geometry, Brush* brush,
                              float width) = 0;
  virtual void FillGeometry(Geometry* geometry, Brush* brush) = 0;

  virtual void DrawText(const Point& offset, TextLayout* text_layout,
                        Brush* brush) = 0;

  virtual void EndDraw() = 0;
};
}  // namespace cru::platform::graph
