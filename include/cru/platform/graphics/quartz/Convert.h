#pragma once
#include "cru/common/Range.h"
#include "cru/common/String.h"
#include "cru/common/io/Stream.h"
#include "cru/platform/Matrix.h"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
CGAffineTransform Convert(const Matrix& matrix);
Matrix Convert(const CGAffineTransform& matrix);

CGPoint Convert(const Point& point);
Point Convert(const CGPoint& point);

CGSize Convert(const Size& size);
Size Convert(const CGSize& size);

CGRect Convert(const Rect& rect);
Rect Convert(const CGRect& rect);

CGDataProviderRef ConvertStreamToCGDataProvider(io::Stream* stream);
CGDataConsumerRef ConvertStreamToCGDataConsumer(io::Stream* stream);
}  // namespace cru::platform::graphics::quartz
