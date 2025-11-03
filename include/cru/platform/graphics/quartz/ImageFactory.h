#pragma once
#include "Base.h"

#include <cru/platform/graphics/ImageFactory.h>

namespace cru::platform::graphics::quartz {
class QuartzImageFactory : public OsxQuartzResource,
                           public virtual IImageFactory {
 public:
  explicit QuartzImageFactory(IGraphicsFactory* graphics_factory);
  ~QuartzImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) override;
  void EncodeToStream(IImage* image, io::Stream* stream, ImageFormat format,
                      float quality) override;
  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;
};
}  // namespace cru::platform::graphics::quartz
