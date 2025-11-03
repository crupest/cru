#pragma once
#include "Base.h"

#include <cru/platform/graphics/Geometry.h>

#include <cairo/cairo.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoGeometry : public CairoResource,
                                                      public virtual IGeometry {
 public:
  CairoGeometry(CairoGraphicsFactory* factory, cairo_path_t* cairo_path,
                const Matrix& transform = Matrix::Identity(),
                bool auto_destroy = true);
  ~CairoGeometry();

  bool StrokeContains(float width, const Point& point) override;
  bool FillContains(const Point& point) override;
  Rect GetBounds() override;
  std::unique_ptr<IGeometry> Transform(const Matrix& matrix) override;
  std::unique_ptr<IGeometry> CreateStrokeGeometry(float width) override;

  cairo_path_t* GetCairoPath() const { return cairo_path_; }

 private:
  cairo_path_t* cairo_path_;
  Matrix transform_;
  bool auto_destroy_;
};

class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoGeometryBuilder
    : public CairoResource,
      public virtual IGeometryBuilder {
 public:
  explicit CairoGeometryBuilder(CairoGraphicsFactory* factory);
  ~CairoGeometryBuilder() override;

 public:
  Point GetCurrentPosition() override;

  void MoveTo(const Point& point) override;
  void LineTo(const Point& point) override;
  void CubicBezierTo(const Point& start_control_point,
                     const Point& end_control_point,
                     const Point& end_point) override;
  void QuadraticBezierTo(const Point& control_point,
                         const Point& end_point) override;

  void CloseFigure(bool close) override;

  std::unique_ptr<IGeometry> Build() override;

 private:
  cairo_surface_t* surface_;
  cairo_t* cairo_;
};
}  // namespace cru::platform::graphics::cairo
