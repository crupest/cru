#include "cru/platform/graphics/cairo/CairoGeometry.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"

#include <cairo/cairo.h>

namespace cru::platform::graphics::cairo {
CairoGeometry::CairoGeometry(CairoGraphicsFactory* factory,
                             cairo_path_t* cairo_path, const Matrix& transform,
                             bool auto_destroy)
    : CairoResource(factory),
      cairo_path_(cairo_path),
      transform_(transform),
      auto_destroy_(auto_destroy) {
  Expects(cairo_path);
}

CairoGeometry::~CairoGeometry() {
  if (auto_destroy_) {
    cairo_path_destroy(cairo_path_);
    cairo_path_ = nullptr;
  }
}

bool CairoGeometry::StrokeContains(float width, const Point& point) {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  auto matrix = Convert(transform_);
  cairo_transform(cairo, &matrix);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  cairo_set_line_width(cairo, width);
  auto result = cairo_in_stroke(cairo, point.x, point.y);
  cairo_restore(cairo);
  return result;
}

bool CairoGeometry::FillContains(const Point& point) {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  auto matrix = Convert(transform_);
  cairo_transform(cairo, &matrix);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  auto result = cairo_in_fill(cairo, point.x, point.y);
  cairo_restore(cairo);
  return result;
}

Rect CairoGeometry::GetBounds() {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  auto matrix = Convert(transform_);
  cairo_transform(cairo, &matrix);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  double l, t, r, b;
  cairo_path_extents(cairo, &l, &t, &r, &b);
  cairo_restore(cairo);
  return Rect::FromVertices(l, t, r, b);
}

std::unique_ptr<IGeometry> CairoGeometry::Transform(const Matrix& matrix) {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  auto path = cairo_copy_path(cairo);
  cairo_restore(cairo);
  return std::unique_ptr<IGeometry>(new CairoGeometry(
      GetCairoGraphicsFactory(), path, transform_ * matrix, true));
}

std::unique_ptr<IGeometry> CairoGeometry::CreateStrokeGeometry(float width) {
  throw Exception("Not implemented");
}

CairoGeometryBuilder::CairoGeometryBuilder(CairoGraphicsFactory* factory)
    : CairoResource(factory) {
  surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
  cairo_ = cairo_create(surface_);
  cairo_new_path(cairo_);
  cairo_move_to(cairo_, 0.0, 0.0);
}

CairoGeometryBuilder::~CairoGeometryBuilder() {
  cairo_destroy(cairo_);
  cairo_surface_destroy(surface_);
}

Point CairoGeometryBuilder::GetCurrentPosition() {
  double x, y;
  cairo_get_current_point(cairo_, &x, &y);
  return Point(x, y);
}

void CairoGeometryBuilder::MoveTo(const Point& point) {
  cairo_move_to(cairo_, point.x, point.y);
}

void CairoGeometryBuilder::LineTo(const Point& point) {
  cairo_line_to(cairo_, point.x, point.y);
}

void CairoGeometryBuilder::CubicBezierTo(const Point& start_control_point,
                                         const Point& end_control_point,
                                         const Point& end_point) {
  cairo_curve_to(cairo_, start_control_point.x, start_control_point.y,
                 end_control_point.x, end_control_point.y, end_point.x,
                 end_point.y);
}

void CairoGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                             const Point& end_point) {
  CubicBezierTo(control_point, control_point, end_point);
}

void CairoGeometryBuilder::CloseFigure(bool close) {
  if (close) cairo_close_path(cairo_);
}

std::unique_ptr<IGeometry> CairoGeometryBuilder::Build() {
  cairo_path_t* path = cairo_copy_path(cairo_);
  return std::unique_ptr<IGeometry>(new CairoGeometry(
      GetCairoGraphicsFactory(), path, Matrix::Identity(), true));
}
}  // namespace cru::platform::graphics::cairo
