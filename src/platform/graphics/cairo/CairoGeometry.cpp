#include "cru/platform/graphics/cairo/CairoGeometry.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"

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

bool CairoGeometry::FillContains(const Point& point) {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  auto result = cairo_in_fill(cairo, point.x, point.y);
  cairo_restore(cairo);
  return result;
}

Rect CairoGeometry::GetBounds() {
  auto cairo = GetCairoGraphicsFactory()->GetDefaultCairo();
  cairo_save(cairo);
  cairo_new_path(cairo);
  cairo_append_path(cairo, cairo_path_);
  double l, t, r, b;
  cairo_path_extents(cairo, &l, &t, &r, &b);
  cairo_restore(cairo);
  return Rect::FromVertices(l, t, r, b);
}

std::unique_ptr<IGeometry> CairoGeometry::Transform(const Matrix& matrix) {

}

std::unique_ptr<IGeometry> CairoGeometry::CreateStrokeGeometry(float width) {
  throw Exception(u"Not implemented");
}
}  // namespace cru::platform::graphics::cairo
