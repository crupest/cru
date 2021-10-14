#include "cru/osx/graphics/quartz/Convert.hpp"
#include <cstdint>

namespace cru::platform::graphics::osx::quartz {

CGPoint Convert(const Point& point) { return CGPoint{point.x, point.y}; }
Point Convert(const CGPoint& point) { return Point(point.x, point.y); }

CGSize Convert(const Size& size) { return CGSize{size.width, size.height}; }
Size Convert(const CGSize& size) { return Size(size.width, size.height); }

CGAffineTransform Convert(const Matrix& matrix) {
  return CGAffineTransformMake(matrix.m11, matrix.m12, matrix.m21, matrix.m22,
                               matrix.m31, matrix.m32);
}

Matrix Convert(const CGAffineTransform& matrix) {
  return Matrix(matrix.a, matrix.b, matrix.c, matrix.d, matrix.tx, matrix.ty);
}

CGRect Convert(const Rect& rect) {
  return CGRect{CGPoint{rect.left, rect.top}, CGSize{rect.width, rect.height}};
}

Rect Convert(const CGRect& rect) {
  return Rect{static_cast<float>(rect.origin.x),
              static_cast<float>(rect.origin.y),
              static_cast<float>(rect.size.width),
              static_cast<float>(rect.size.height)};
}
}  // namespace cru::platform::graphics::osx::quartz
