#include "cru/win/graphics/direct/Image.hpp"
#include "cru/common/platform/win/Exception.hpp"
#include "cru/win/graphics/direct/ConvertUtil.hpp"
#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"

namespace cru::platform::graphics::win::direct {
Direct2DImage::Direct2DImage(DirectGraphicsFactory* graphics_factory,
                             Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap)
    : DirectGraphicsResource(graphics_factory),
      d2d_bitmap_(std::move(d2d_bitmap)) {}

Direct2DImage::~Direct2DImage() {}

float Direct2DImage::GetWidth() { return d2d_bitmap_->GetSize().width; }

float Direct2DImage::GetHeight() { return d2d_bitmap_->GetSize().height; }

std::unique_ptr<IImage> Direct2DImage::CreateWithRect(const Rect& rect) {
  auto device_context = GetDirectFactory()->CreateD2D1DeviceContext();
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
  ThrowIfFailed(device_context->CreateBitmap(
      D2D1::SizeU(rect.width, rect.height), nullptr, 0,
      D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET), &bitmap));
  device_context->SetTarget(bitmap.Get());
  device_context->BeginDraw();
  device_context->DrawBitmap(d2d_bitmap_.Get(), Convert(rect));
  ThrowIfFailed(device_context->EndDraw());
  return std::make_unique<Direct2DImage>(GetDirectFactory(), std::move(bitmap));
}

}  // namespace cru::platform::graphics::win::direct