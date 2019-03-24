#pragma once
#include "pre.hpp"

#include <Windows.h>
#include <memory>

#include "base.hpp"

struct IDXGISwapChain1;
struct ID2D1Bitmap1;

namespace cru::graph {
class GraphManager;

// Represents a window render target.
class WindowRenderTarget : public Object {
 public:
  WindowRenderTarget(GraphManager* graph_manager, HWND hwnd);
  WindowRenderTarget(const WindowRenderTarget& other) = delete;
  WindowRenderTarget(WindowRenderTarget&& other) = delete;
  WindowRenderTarget& operator=(const WindowRenderTarget& other) = delete;
  WindowRenderTarget& operator=(WindowRenderTarget&& other) = delete;
  ~WindowRenderTarget() override = default;

 public:
  // Get the graph manager that created the render target.
  GraphManager* GetGraphManager() const { return graph_manager_; }

  // Get the target bitmap which can be set as the ID2D1DeviceContext's target.
  ID2D1Bitmap1* GetTargetBitmap() const { return target_bitmap_.get(); }

  // Resize the underlying buffer.
  void ResizeBuffer(int width, int height);

  // Set this render target as the d2d device context's target.
  void SetAsTarget();

  // Present the data of the underlying buffer to the window.
  void Present();

 private:
  void CreateTargetBitmap();

 private:
  GraphManager* graph_manager_;
  std::shared_ptr<IDXGISwapChain1> dxgi_swap_chain_;
  std::shared_ptr<ID2D1Bitmap1> target_bitmap_;
};
}  // namespace cru::graph
