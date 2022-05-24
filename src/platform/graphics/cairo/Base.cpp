#include "cru/platform/graphics/cairo/Base.h"
#include <cairo/cairo.h>

namespace cru::platform::graphics::cairo {
cairo_matrix_t Convert(const Matrix& matrix) {
  cairo_matrix_t m;
  cairo_matrix_init(&m, matrix.m11, matrix.m12, matrix.m21, matrix.m22,
                    matrix.m31, matrix.m32);
  return m;
}

Matrix Convert(const cairo_matrix_t* matrix) {
  return Matrix(matrix->xx, matrix->yx, matrix->xy, matrix->yy, matrix->x0,
                matrix->y0);
}

Matrix Convert(const cairo_matrix_t& matrix) { return Convert(&matrix); }
}  // namespace cru::platform::graphics::cairo
