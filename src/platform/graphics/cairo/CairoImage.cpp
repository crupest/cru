#include "cru/platform/graphics/cairo/CairoImage.h"
#include "cru/platform/graphics/cairo/CairoPainter.h"

#include <memory>

namespace cru::platform::graphics::cairo {
CairoImage::CairoImage(CairoGraphicsFactory* factory, int width, int height)
    : CairoResource(factory) {
  cairo_surface_ =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  auto_release_ = true;
}

CairoImage::CairoImage(CairoGraphicsFactory* factory,
                       cairo_surface_t* cairo_image_surface, bool auto_release)
    : CairoResource(factory) {
  Expects(cairo_surface_get_type(cairo_image_surface) ==
          CAIRO_SURFACE_TYPE_IMAGE);
  cairo_surface_ = cairo_image_surface;
  auto_release_ = auto_release;
}

CairoImage::~CairoImage() {
  if (auto_release_) {
    cairo_surface_destroy(cairo_surface_);
  }
}

float CairoImage::GetWidth() {
  return cairo_image_surface_get_width(cairo_surface_);
}

float CairoImage::GetHeight() {
  return cairo_image_surface_get_height(cairo_surface_);
}

std::unique_ptr<IImage> CairoImage::CreateWithRect(const Rect& rect) {
  auto surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, rect.width, rect.height);
  auto cairo = cairo_create(surface);
  cairo_set_source_surface(cairo, cairo_surface_, rect.left, rect.top);
  cairo_paint(cairo);
  cairo_destroy(cairo);
  return std::make_unique<CairoImage>(GetCairoGraphicsFactory(), surface, true);
}

std::unique_ptr<IPainter> CairoImage::CreatePainter() {
  auto cairo = cairo_create(cairo_surface_);
  return std::make_unique<CairoPainter>(GetCairoGraphicsFactory(), cairo, true);
}

}  // namespace cru::platform::graphics::cairo
