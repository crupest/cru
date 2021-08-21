#include "cru/osx/graphics/quartz/Convert.hpp"

namespace cru::platform::graphics::osx::quartz {
CGAffineTransform Convert(const Matrix& matrix) {
  return CGAffineTransformMake(matrix.m11, matrix.m12, matrix.m21, matrix.m22, matrix.m31, matrix.m32);
}

Matrix Convert(const CGAffineTransform& matrix) {
  return Matrix(matrix.a, matrix.b, matrix.c, matrix.d, matrix.tx, matrix.ty);
}
}  // namespace cru::platform::graphics::osx::quartz
