#include "cru/platform/graphics/direct2d/Image.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/graphics/direct2d/Painter.h"

namespace cru::platform::graphics::direct2d {
Direct2DImage::Direct2DImage(DirectGraphicsFactory* graphics_factory,
                             Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap)
    : DirectGraphicsResource(graphics_factory),
      d2d_bitmap_(std::move(d2d_bitmap)) {
  Expects(d2d_bitmap_);
}

Direct2DImage::~Direct2DImage() {}

float Direct2DImage::GetWidth() { return d2d_bitmap_->GetSize().width; }

float Direct2DImage::GetHeight() { return d2d_bitmap_->GetSize().height; }

std::unique_ptr<IImage> Direct2DImage::CreateWithRect(const Rect& rect) {
  auto device_context = GetDirectFactory()->CreateD2D1DeviceContext();
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
  CheckHResult(device_context->CreateBitmap(
      D2D1::SizeU(rect.width, rect.height), nullptr, 0,
      D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                              D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                                D2D1_ALPHA_MODE_PREMULTIPLIED)),
      &bitmap));
  device_context->SetTarget(bitmap.Get());
  device_context->BeginDraw();
  device_context->DrawBitmap(d2d_bitmap_.Get(), Convert(rect));
  CheckHResult(device_context->EndDraw());
  return std::make_unique<Direct2DImage>(GetDirectFactory(), std::move(bitmap));
}

std::unique_ptr<IPainter> Direct2DImage::CreatePainter() {
  auto device_context = GetDirectFactory()->CreateD2D1DeviceContext();
  device_context->SetTarget(d2d_bitmap_.Get());
  return std::make_unique<D2DDeviceContextPainter>(
      GetDirectFactory(), device_context.Detach(), true);
}
}  // namespace cru::platform::graphics::direct2d
