#pragma once
#include "cru/platform/Matrix.hpp"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
  CGAffineTransform Convert(const Matrix& matrix);
  Matrix Convert(const CGAffineTransform& matrix);
}
