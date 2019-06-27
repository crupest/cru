#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"

namespace cru::platform::graph::win::direct {
struct IDirectFactory;
}

namespace cru::platform::native::win {
// Represents a window render target.
class WindowRenderTarget : public Object {
 public:
  WindowRenderTarget(graph::win::direct::IDirectFactory* factory, HWND hwnd);
  WindowRenderTarget(const WindowRenderTarget& other) = delete;
  WindowRenderTarget(WindowRenderTarget&& other) = delete;
  WindowRenderTarget& operator=(const WindowRenderTarget& other) = delete;
  WindowRenderTarget& operator=(WindowRenderTarget&& other) = delete;
  ~WindowRenderTarget() override = default;

 public:
  graph::win::direct::IDirectFactory* GetWinNativeFactory() const {
    return factory_;
  }

  // Get the target bitmap which can be set as the ID2D1DeviceContext's target.
  ID2D1Bitmap1* GetTargetBitmap() const { return target_bitmap_.Get(); }

  // Resize the underlying buffer.
  void ResizeBuffer(int width, int height);

  // Set this render target as the d2d device context's target.
  void SetAsTarget();

  // Present the data of the underlying buffer to the window.
  void Present();

 private:
  void CreateTargetBitmap();

 private:
  graph::win::direct::IDirectFactory* factory_;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
};
}  // namespace cru::platform::native::win
