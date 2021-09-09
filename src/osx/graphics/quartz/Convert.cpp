#include "cru/osx/graphics/quartz/Convert.hpp"
#include <cstdint>

namespace cru::platform::graphics::osx::quartz {
CFStringRef Convert(const String& string) {
  return CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(std::uint16_t), kCFStringEncodingUTF16, false);
}

String Convert(CFStringRef string) {
  auto d = CFStringCreateExternalRepresentation(nullptr, string,
                                                kCFStringEncodingUTF16, 0);
  auto l = CFDataGetLength(d);

  auto s = String::FromUtf16(
      reinterpret_cast<const std::uint16_t*>(CFDataGetBytePtr(d)),
      CFDataGetLength(d) / 2);

  CFRelease(d);

  return s;
}

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

CFRange Convert(const Range& range) {
  return CFRangeMake(range.position, range.count);
}

Range Convert(const CFRange& range) {
  return Range(range.location, range.length);
}
}  // namespace cru::platform::graphics::osx::quartz
