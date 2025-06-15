#pragma once
#include "Resource.h"
#include "cru/graphics/ImageFactory.h"

namespace cru::graphics::quartz {
class QuartzImageFactory : public OsxQuartzResource,
                           public virtual IImageFactory {
 public:
  explicit QuartzImageFactory(IGraphicsFactory* graphics_factory);


  ~QuartzImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(Stream* stream) override;
  void EncodeToStream(IImage* image, Stream* stream, ImageFormat format,
                      float quality) override;
  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;
};
}  // namespace cru::graphics::quartz
