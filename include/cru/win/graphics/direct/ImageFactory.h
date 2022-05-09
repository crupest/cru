#pragma once

#include "Resource.h"
#include "cru/platform/graphics/Image.h"
#include "cru/platform/graphics/ImageFactory.h"

namespace cru::platform::graphics::win::direct {
class CRU_WIN_GRAPHICS_DIRECT_API WinImageFactory
    : public DirectGraphicsResource,
      public virtual IImageFactory {
 public:
  explicit WinImageFactory(DirectGraphicsFactory* graphics_factory);

  CRU_DELETE_COPY(WinImageFactory)
  CRU_DELETE_MOVE(WinImageFactory)

  ~WinImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) override;

  void EncodeToStream(IImage* image, io::Stream* stream, ImageFormat format,
                      float quality) override;

  std::unique_ptr<IImage> CreateBitmap(int width, int height) override;

 private:
  Microsoft::WRL::ComPtr<IWICImagingFactory> wic_imaging_factory_;
};
}  // namespace cru::platform::graphics::win::direct
