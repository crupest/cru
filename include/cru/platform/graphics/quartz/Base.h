#pragma once
#include <cru/base/Base.h>
#include <cru/base/io/Stream.h>
#include <cru/platform/graphics/Base.h>

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
class OsxQuartzResource : public Object, public virtual IGraphicsResource {
 public:
  explicit OsxQuartzResource(IGraphicsFactory* graphics_factory)
      : graphics_factory_(graphics_factory) {}

 public:
  std::string GetPlatformId() const override { return "OSX Quartz"; }

  IGraphicsFactory* GetGraphicsFactory() override { return graphics_factory_; }

 private:
  IGraphicsFactory* graphics_factory_;
};

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
