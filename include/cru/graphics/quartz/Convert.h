#pragma once
#include <cru/Range.h>
#include <cru/String.h>
#include <cru/Stream.h>
#include "cru/Matrix.h"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::graphics::quartz {
CGAffineTransform Convert(const Matrix& matrix);
Matrix Convert(const CGAffineTransform& matrix);

CGPoint Convert(const Point& point);
Point Convert(const CGPoint& point);

CGSize Convert(const Size& size);
Size Convert(const CGSize& size);

CGRect Convert(const Rect& rect);
Rect Convert(const CGRect& rect);

CGDataProviderRef ConvertStreamToCGDataProvider(Stream* stream);
CGDataConsumerRef ConvertStreamToCGDataConsumer(Stream* stream);
}  // namespace cru::graphics::quartz
