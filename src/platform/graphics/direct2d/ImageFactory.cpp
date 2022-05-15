#include "cru/platform/graphics/direct2d/ImageFactory.h"
#include "cru/common/platform/win/Exception.h"
#include "cru/common/platform/win/StreamConvert.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/direct2d/Exception.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/graphics/direct2d/Image.h"

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace cru::platform::graphics::direct2d {
WinImageFactory::WinImageFactory(DirectGraphicsFactory* graphics_factory)
    : DirectGraphicsResource(graphics_factory) {
  HRESULT hr =
      CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&wic_imaging_factory_));
  ThrowIfFailed(hr);
}

WinImageFactory::~WinImageFactory() {}

std::unique_ptr<IImage> WinImageFactory::DecodeFromStream(io::Stream* stream) {
  auto graphics_factory = GetDirectFactory();

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

  auto d2d_context = graphics_factory->GetDefaultD2D1DeviceContext();

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_image;
  d2d_context->CreateBitmapFromWicBitmap(
      wic_bitmap_frame_decode.Get(),
      D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                              D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                                D2D1_ALPHA_MODE_PREMULTIPLIED)),
      &d2d_image);

  return std::make_unique<Direct2DImage>(graphics_factory,
                                         std::move(d2d_image));
}

GUID ConvertImageFormatToGUID(ImageFormat format) {
  GUID format_guid;
  switch (format) {
    case ImageFormat::Png:
      format_guid = GUID_ContainerFormatPng;
      break;
    case ImageFormat::Jpeg:
      format_guid = GUID_ContainerFormatJpeg;
      break;
    case ImageFormat::Gif:
      format_guid = GUID_ContainerFormatGif;
      break;
    default:
      throw Exception(u"Unknown image format");
  }
  return format_guid;
}

void WinImageFactory::EncodeToStream(IImage* image, io::Stream* stream,
                                     ImageFormat format, float quality) {
  auto direct_image = CheckPlatform<Direct2DImage>(image, GetPlatformId());

  Microsoft::WRL::ComPtr<IStream> com_stream(
      platform::win::ConvertStreamToComStream(stream));

  auto d2d_bitmap = direct_image->GetD2DBitmap();
  auto size = d2d_bitmap->GetPixelSize();
  FLOAT dpi_x, dpi_y;
  d2d_bitmap->GetDpi(&dpi_x, &dpi_y);
  auto pixel_format = d2d_bitmap->GetPixelFormat();
  Ensures(pixel_format.format == DXGI_FORMAT_B8G8R8A8_UNORM);
  Ensures(pixel_format.alphaMode == D2D1_ALPHA_MODE_PREMULTIPLIED);

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> cpu_bitmap;
  ThrowIfFailed(GetDirectFactory()->GetDefaultD2D1DeviceContext()->CreateBitmap(
      size, nullptr, 0,
      D2D1::BitmapProperties1(
          D2D1_BITMAP_OPTIONS_CANNOT_DRAW | D2D1_BITMAP_OPTIONS_CPU_READ,
          pixel_format, dpi_x, dpi_y),
      &cpu_bitmap));

  ThrowIfFailed(cpu_bitmap->CopyFromBitmap(nullptr, d2d_bitmap.Get(), nullptr));

  D2D1_MAPPED_RECT mapped_rect;
  ThrowIfFailed(cpu_bitmap->Map(D2D1_MAP_OPTIONS_READ, &mapped_rect));

  Microsoft::WRL::ComPtr<IWICBitmap> wic_bitmap;
  ThrowIfFailed(wic_imaging_factory_->CreateBitmapFromMemory(
      size.width, size.height, GUID_WICPixelFormat32bppPBGRA, mapped_rect.pitch,
      mapped_rect.pitch * size.height, mapped_rect.bits, &wic_bitmap));

  ThrowIfFailed(cpu_bitmap->Unmap());

  Microsoft::WRL::ComPtr<IWICBitmapEncoder> wic_bitmap_encoder;

  ThrowIfFailed(wic_imaging_factory_->CreateEncoder(
      ConvertImageFormatToGUID(format), nullptr, &wic_bitmap_encoder));

  ThrowIfFailed(wic_bitmap_encoder->Initialize(com_stream.Get(),
                                               WICBitmapEncoderNoCache));

  Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> wic_bitmap_frame_encode;
  Microsoft::WRL::ComPtr<IPropertyBag2> property_bag;
  ThrowIfFailed(wic_bitmap_encoder->CreateNewFrame(&wic_bitmap_frame_encode,
                                                   &property_bag));

  if (format == ImageFormat::Jpeg) {
    PROPBAG2 option = {0};
    option.pstrName = const_cast<wchar_t*>(L"ImageQuality");
    VARIANT varValue;
    VariantInit(&varValue);
    varValue.vt = VT_R4;
    varValue.fltVal = quality;
    ThrowIfFailed(property_bag->Write(1, &option, &varValue));
  }

  ThrowIfFailed(wic_bitmap_frame_encode->Initialize(property_bag.Get()));
  ThrowIfFailed(wic_bitmap_frame_encode->SetResolution(dpi_x, dpi_y));
  ThrowIfFailed(wic_bitmap_frame_encode->WriteSource(wic_bitmap.Get(), NULL));
  ThrowIfFailed(wic_bitmap_frame_encode->Commit());

  ThrowIfFailed(wic_bitmap_encoder->Commit());
}

std::unique_ptr<IImage> WinImageFactory::CreateBitmap(int width, int height) {
  if (width <= 0) throw Exception(u"Bitmap width must be greater than 0.");
  if (height <= 0) throw Exception(u"Bitmap height must be greater than 0.");

  auto graphics_factory = GetDirectFactory();

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;

  auto d2d_context = graphics_factory->GetDefaultD2D1DeviceContext();
  ThrowIfFailed(d2d_context->CreateBitmap(
      D2D1::SizeU(width, height), nullptr, 0,
      D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                              D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                                D2D1_ALPHA_MODE_PREMULTIPLIED)),
      &bitmap));

  return std::make_unique<Direct2DImage>(graphics_factory, std::move(bitmap));
}
}  // namespace cru::platform::graphics::direct2d
