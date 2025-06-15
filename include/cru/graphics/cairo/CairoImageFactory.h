#pragma once
#include "../ImageFactory.h"
#include "CairoResource.h"

namespace cru::graphics::cairo {
class CairoImageFactory : public CairoResource, public virtual IImageFactory {
 public:
  CairoImageFactory(CairoGraphicsFactory* factory);
  ~CairoImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(Stream* stream) override;
  void EncodeToStream(IImage* image, Stream* stream, ImageFormat format,
                      float quality) override;

  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;
};
}  // namespace cru::graphics::cairo
