#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/CairoResource.h"

namespace cru::platform::graphics::cairo {
CairoGraphicsFactory::CairoGraphicsFactory() : CairoResource(this) {
  default_cairo_surface_ =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
  default_cairo_ = cairo_create(default_cairo_surface_);
}

CairoGraphicsFactory::~CairoGraphicsFactory() {
  cairo_destroy(default_cairo_);
  cairo_surface_destroy(default_cairo_surface_);
}
}  // namespace cru::platform::graphics::cairo
