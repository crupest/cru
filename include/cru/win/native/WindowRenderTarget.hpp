#pragma once
#include "Base.hpp"

namespace cru::platform::graph::win::direct {
class DirectGraphFactory;
}

namespace cru::platform::native::win {
// Represents a window render target.
class WindowRenderTarget : public Object {
 public:
  WindowRenderTarget(graph::win::direct::DirectGraphFactory* factory,
                     WinNativeWindow* window);

  CRU_DELETE_COPY(WindowRenderTarget)
  CRU_DELETE_MOVE(WindowRenderTarget)

  ~WindowRenderTarget() override = default;

 public:
  graph::win::direct::DirectGraphFactory* GetDirectFactory() const {
    return factory_;
  }

  ID2D1DeviceContext* GetD2D1DeviceContext() {
    return d2d1_device_context_.Get();
  }

  // Resize the underlying buffer.
  void ResizeBuffer(int width, int height);

  // Set this render target as the d2d device context's target.
  void SetAsTarget();

  // Present the data of the underlying buffer to the window.
  void Present();

 private:
  void CreateTargetBitmap();

 private:
  WinNativeWindow* window_;
  graph::win::direct::DirectGraphFactory* factory_;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
};
}  // namespace cru::platform::native::win
