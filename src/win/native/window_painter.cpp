#include "window_painter.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/native/window_render_target.hpp"

#include <cassert>

namespace cru::win::native {
WindowPainter::WindowPainter(WinNativeWindow* window)
    : D2DPainter(window->GetWindowRenderTarget()
                     ->GetGraphManager()
                     ->GetD2D1DeviceContext()),
      window_(window) {
  window->GetWindowRenderTarget()->SetAsTarget();
  window->GetWindowRenderTarget()
      ->GetGraphManager()
      ->GetD2D1DeviceContext()
      ->BeginDraw();
}

WindowPainter::~WindowPainter() { EndDraw(); }

void WindowPainter::DoEndDraw() {
  ThrowIfFailed(window_->GetWindowRenderTarget()
                    ->GetGraphManager()
                    ->GetD2D1DeviceContext()
                    ->EndDraw());
  window_->GetWindowRenderTarget()->Present();
}
}  // namespace cru::win::native
