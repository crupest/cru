#pragma once
#include "cru/common/Range.hpp"
#include "cru/common/String.hpp"
#include "cru/common/io/Stream.hpp"
#include "cru/platform/Matrix.hpp"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
CGAffineTransform Convert(const Matrix& matrix);
Matrix Convert(const CGAffineTransform& matrix);

CGPoint Convert(const Point& point);
Point Convert(const CGPoint& point);

CGSize Convert(const Size& size);
Size Convert(const CGSize& size);

CGRect Convert(const Rect& rect);
Rect Convert(const CGRect& rect);

CGDataProviderRef ConvertStreamToCGDataProvider(io::Stream* stream);
}  // namespace cru::platform::graphics::osx::quartz
