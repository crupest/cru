#pragma once

#include "Resource.hpp"
#include "cru/platform/graphics/Image.hpp"
#include "cru/platform/graphics/ImageFactory.hpp"

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

 private:
  DirectGraphicsFactory* graphics_factory_;

  Microsoft::WRL::ComPtr<IWICImagingFactory> wic_imaging_factory_;
};
}  // namespace cru::platform::graphics::win::direct
