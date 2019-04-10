#include "window_painter.hpp"

#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/native/window_render_target.hpp"

#include <cassert>

namespace cru::win::native {
WindowPainter::WindowPainter(WinNativeWindow* window)
    : WinPainter(window->GetWindowRenderTarget()
                     ->GetGraphManager()
                     ->GetD2D1DeviceContext()),
      window_(window) {
  window->GetWindowRenderTarget()->SetAsTarget();
  window->GetWindowRenderTarget()
      ->GetGraphManager()
      ->GetD2D1DeviceContext()
      ->BeginDraw();
}

void WindowPainter::DoEndDraw() {
  WinPainter::DoEndDraw();
  window_->GetWindowRenderTarget()->Present(); }
}  // namespace cru::win::native
