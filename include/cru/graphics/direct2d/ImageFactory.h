#pragma once

#include "Resource.h"
#include "cru/graphics/Image.h"
#include "cru/graphics/ImageFactory.h"

namespace cru::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API WinImageFactory
    : public DirectGraphicsResource,
      public virtual IImageFactory {
 public:
  explicit WinImageFactory(DirectGraphicsFactory* graphics_factory);


  ~WinImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(Stream* stream) override;

  void EncodeToStream(IImage* image, Stream* stream, ImageFormat format,
                      float quality) override;

  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;

 private:
  Microsoft::WRL::ComPtr<IWICImagingFactory> wic_imaging_factory_;
};
}  // namespace cru::graphics::direct2d
