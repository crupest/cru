#include "cru/win/graphics/direct/ImageFactory.hpp"
#include "cru/common/platform/win/StreamConvert.hpp"
#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"
#include "cru/win/graphics/direct/Image.hpp"

#include <wincodec.h>

namespace cru::platform::graphics::win::direct {
WinImageFactory::WinImageFactory(DirectGraphicsFactory* graphics_factory)
    : DirectGraphicsResource(graphics_factory) {
  HRESULT hr =
      CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&wic_imaging_factory_));
  ThrowIfFailed(hr);
}

WinImageFactory::~WinImageFactory() {}

std::unique_ptr<IImage> WinImageFactory::DecodeFromStream(io::Stream* stream) {
  HRESULT hr;

  Microsoft::WRL::ComPtr<IStream> com_stream(
      platform::win::ConvertStreamToComStream(stream));

  Microsoft::WRL::ComPtr<IWICBitmapDecoder> wic_bitmap_decoder;
  hr = wic_imaging_factory_->CreateDecoderFromStream(
      com_stream.Get(), NULL, WICDecodeMetadataCacheOnDemand,
      &wic_bitmap_decoder);
  ThrowIfFailed(hr);

  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> wic_bitmap_frame_decode;
  hr = wic_bitmap_decoder->GetFrame(0, &wic_bitmap_frame_decode);
  ThrowIfFailed(hr);

  auto d2d_context = graphics_factory_->GetDefaultD2D1DeviceContext();

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_image;
  d2d_context->CreateBitmapFromWicBitmap(wic_bitmap_frame_decode.Get(), NULL,
                                         &d2d_image);

  return std::make_unique<Direct2DImage>(graphics_factory_,
                                         std::move(d2d_image));
}
}  // namespace cru::platform::graphics::win::direct
