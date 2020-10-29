#include "cru/win/graphics/direct/WindowRenderTarget.hpp"

#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"

namespace cru::platform::graphics::win::direct {
D2DWindowRenderTarget::D2DWindowRenderTarget(
    gsl::not_null<DirectGraphFactory*> factory, HWND hwnd)
    : factory_(factory), hwnd_(hwnd) {
  const auto d3d11_device = factory->GetD3D11Device();
  const auto dxgi_factory = factory->GetDxgiFactory();

  d2d1_device_context_ = factory->CreateD2D1DeviceContext();
  d2d1_device_context_->SetUnitMode(D2D1_UNIT_MODE_DIPS);

  // Allocate a descriptor.
  DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
  swap_chain_desc.Width = 0;  // use automatic sizing
  swap_chain_desc.Height = 0;
  swap_chain_desc.Format =
      DXGI_FORMAT_B8G8R8A8_UNORM;  // this is the most common swapchain format
  swap_chain_desc.Stereo = false;
  swap_chain_desc.SampleDesc.Count = 1;  // don't use multi-sampling
  swap_chain_desc.SampleDesc.Quality = 0;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.BufferCount = 2;  // use double buffering to enable flip
  swap_chain_desc.Scaling = DXGI_SCALING_NONE;
  swap_chain_desc.SwapEffect =
      DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;  // all apps must use this SwapEffect
  swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  swap_chain_desc.Flags = 0;

  // Get the final swap chain for this window from the DXGI factory.
  ThrowIfFailed(dxgi_factory->CreateSwapChainForHwnd(
      d3d11_device, hwnd, &swap_chain_desc, nullptr, nullptr,
      &dxgi_swap_chain_));

  CreateTargetBitmap();
}

void D2DWindowRenderTarget::SetDpi(float x, float y) {
  d2d1_device_context_->SetDpi(x, y);
}

void D2DWindowRenderTarget::ResizeBuffer(const int width, const int height) {
  // In order to resize buffer, we need to untarget the buffer first.
  d2d1_device_context_->SetTarget(nullptr);
  target_bitmap_ = nullptr;
  ThrowIfFailed(dxgi_swap_chain_->ResizeBuffers(0, width, height,
                                                DXGI_FORMAT_UNKNOWN, 0));
  CreateTargetBitmap();
}

void D2DWindowRenderTarget::Present() {
  ThrowIfFailed(dxgi_swap_chain_->Present(1, 0));
}

void D2DWindowRenderTarget::CreateTargetBitmap() {
  Expects(target_bitmap_ == nullptr);  // target bitmap must not exist.

  // Direct2D needs the dxgi version of the backbuffer surface pointer.
  Microsoft::WRL::ComPtr<IDXGISurface> dxgi_back_buffer;
  ThrowIfFailed(
      dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&dxgi_back_buffer)));

  float dpi_x, dpi_y;
  d2d1_device_context_->GetDpi(&dpi_x, &dpi_y);

  auto bitmap_properties = D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
      dpi_x, dpi_y);

  // Get a D2D surface from the DXGI back buffer to use as the D2D render
  // target.
  ThrowIfFailed(d2d1_device_context_->CreateBitmapFromDxgiSurface(
      dxgi_back_buffer.Get(), &bitmap_properties, &target_bitmap_));

  d2d1_device_context_->SetTarget(target_bitmap_.Get());
}
}  // namespace cru::platform::graphics::win::direct
