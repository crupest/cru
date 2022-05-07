#include "cru/win/graphics/direct/ImageFactory.h"
#include "cru/common/platform/win/Exception.h"
#include "cru/common/platform/win/StreamConvert.h"
#include "cru/win/graphics/direct/Exception.h"
#include "cru/win/graphics/direct/Factory.h"
#include "cru/win/graphics/direct/Image.h"

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

std::unique_ptr<IImage> WinImageFactory::CreateBitmap(int width, int height) {
  if (width <= 0) throw Exception(u"Bitmap width must be greater than 0.");
  if (height <= 0) throw Exception(u"Bitmap height must be greater than 0.");

  Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;

  auto d2d_context = graphics_factory_->GetDefaultD2D1DeviceContext();
  d2d_context->CreateBitmap(
      D2D1::SizeU(width, height),
      D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_TYPELESS,
                                               D2D1_ALPHA_MODE_STRAIGHT)),
      &bitmap);

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap1;
  ThrowIfFailed(bitmap.As(&bitmap1), "Failed to convert bitmap to bitmap1.");

  return std::make_unique<Direct2DImage>(graphics_factory_, std::move(bitmap1));
}
}  // namespace cru::platform::graphics::win::direct
