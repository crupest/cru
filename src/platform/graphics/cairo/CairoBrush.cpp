#include "cru/platform/graphics/cairo/CairoBrush.h"
#include "cru/platform/Color.h"

namespace cru::platform::graphics::cairo {
CairoBrush::CairoBrush(CairoGraphicsFactory* factory)
    : CairoResource(factory) {}

CairoBrush::~CairoBrush() {}

CairoSolidColorBrush::CairoSolidColorBrush(CairoGraphicsFactory* factory)
    : CairoBrush(factory), color_(colors::black) {
  pattern_ =
      cairo_pattern_create_rgba(color_.GetFloatRed(), color_.GetFloatGreen(),
                                color_.GetFloatBlue(), color_.GetFloatAlpha());
}

CairoSolidColorBrush::~CairoSolidColorBrush() {
  cairo_pattern_destroy(pattern_);
}

Color CairoSolidColorBrush::GetColor() { return color_; }

void CairoSolidColorBrush::SetColor(const Color& color) {
  color_ = color;
  cairo_pattern_destroy(pattern_);
  pattern_ =
      cairo_pattern_create_rgba(color_.GetFloatRed(), color_.GetFloatGreen(),
                                color_.GetFloatBlue(), color_.GetFloatAlpha());
}
}  // namespace cru::platform::graphics::cairo
