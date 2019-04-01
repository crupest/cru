#include "cru/platform/win/window_render_target.hpp"

#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/graph_manager.hpp"
#include "dpi_util.hpp"

#include <cassert>

namespace cru::platform::win {
WindowRenderTarget::WindowRenderTarget(GraphManager* graph_manager, HWND hwnd) {
  this->graph_manager_ = graph_manager;

  const auto d3d11_device = graph_manager->GetD3D11Device();
  const auto dxgi_factory = graph_manager->GetDxgiFactory();

  // Allocate a descriptor.
  DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {0};
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
  swap_chain_desc.Flags = 0;

  // Get the final swap chain for this window from the DXGI factory.
  ThrowIfFailed(dxgi_factory->CreateSwapChainForHwnd(
      d3d11_device, hwnd, &swap_chain_desc, nullptr, nullptr,
      &dxgi_swap_chain_));

  CreateTargetBitmap();
}

void WindowRenderTarget::ResizeBuffer(const int width, const int height) {
  const auto graph_manager = graph_manager_;
  const auto d2d1_device_context = graph_manager->GetD2D1DeviceContext();

  Microsoft::WRL::ComPtr<ID2D1Image> old_target;
  d2d1_device_context->GetTarget(&old_target);
  const auto target_this = old_target == this->target_bitmap_;
  if (target_this) d2d1_device_context->SetTarget(nullptr);

  old_target = nullptr;
  target_bitmap_ = nullptr;

  ThrowIfFailed(dxgi_swap_chain_->ResizeBuffers(0, width, height,
                                                DXGI_FORMAT_UNKNOWN, 0));

  CreateTargetBitmap();

  if (target_this) d2d1_device_context->SetTarget(target_bitmap_.Get());
}

void WindowRenderTarget::SetAsTarget() {
  graph_manager_->GetD2D1DeviceContext()->SetTarget(target_bitmap_.Get());
}

void WindowRenderTarget::Present() {
  ThrowIfFailed(dxgi_swap_chain_->Present(1, 0));
}

void WindowRenderTarget::CreateTargetBitmap() {
  assert(target_bitmap_ == nullptr);  // target bitmap must not exist.

  // Direct2D needs the dxgi version of the backbuffer surface pointer.
  Microsoft::WRL::ComPtr<IDXGISurface> dxgi_back_buffer;
  ThrowIfFailed(
      dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&dxgi_back_buffer)));

  const auto dpi = GetDpi();

  auto bitmap_properties = D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
      dpi.x, dpi.y);

  // Get a D2D surface from the DXGI back buffer to use as the D2D render
  // target.
  ThrowIfFailed(
      graph_manager_->GetD2D1DeviceContext()->CreateBitmapFromDxgiSurface(
          dxgi_back_buffer.Get(), &bitmap_properties, &target_bitmap_));
}
}  // namespace cru::platform::win
