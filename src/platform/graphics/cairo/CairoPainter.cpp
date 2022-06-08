#include "cru/platform/graphics/cairo/CairoPainter.h"
#include "cru/platform/Exception.h"
#include "cru/platform/graphics/cairo/Base.h"
#include "cru/platform/graphics/cairo/CairoResource.h"

namespace cru::platform::graphics::cairo {
CairoPainter::CairoPainter(CairoGraphicsFactory* factory, cairo_t* cairo,
                           bool auto_release)
    : CairoResource(factory), cairo_(cairo), auto_release_(auto_release) {}

CairoPainter::~CairoPainter() {
  if (auto_release_) {
    cairo_destroy(cairo_);
  }
}

Matrix CairoPainter::GetTransform() {
  CheckValidation();
  cairo_matrix_t matrix;
  cairo_get_matrix(cairo_, &matrix);
  return Convert(matrix);
}

void CairoPainter::SetTransform(const Matrix& matrix) {
  CheckValidation();
  auto m = Convert(matrix);
  cairo_set_matrix(cairo_, &m);
}

void CairoPainter::ConcatTransform(const Matrix& matrix) {
  CheckValidation();
  auto m = Convert(matrix);
  cairo_transform(cairo_, &m);
}

void CairoPainter::Clear(const Color& color) {
  CheckValidation();
  cairo_set_source_rgba(cairo_, color.GetFloatRed(), color.GetFloatGreen(),
                        color.GetFloatBlue(), color.GetFloatAlpha());
  cairo_paint(cairo_);
}

void CairoPainter::CheckValidation() {
  if (!valid_) {
    throw ReuseException(u"Painter already ended drawing.");
  }
}
}  // namespace cru::platform::graphics::cairo
