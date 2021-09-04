#pragma once
#include "cru/platform/Matrix.hpp"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
CFStringRef Convert(const String& string);
String Convert(CFStringRef string);

CGAffineTransform Convert(const Matrix& matrix);
Matrix Convert(const CGAffineTransform& matrix);

CGRect Convert(const Rect& rect);
Rect Convert(const CGRect& rect);
}  // namespace cru::platform::graphics::osx::quartz
