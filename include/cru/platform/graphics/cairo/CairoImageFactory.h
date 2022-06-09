#pragma once
#include "../ImageFactory.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class CairoImageFactory : public CairoResource, public virtual IImageFactory {
 public:
  CairoImageFactory(CairoGraphicsFactory* factory);
  ~CairoImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) override;
  void EncodeToStream(IImage* image, io::Stream* stream, ImageFormat format,
                      float quality) override;

  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;
};
}  // namespace cru::platform::graphics::cairo
