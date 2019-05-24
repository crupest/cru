#include "window_painter.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/win_native_factory.hpp"
#include "cru/win/native/window_render_target.hpp"

#include <cassert>

namespace cru::win::native {
WindowPainter::WindowPainter(WinNativeWindow* window)
    : WinPainter(window->GetWindowRenderTarget()
                     ->GetWinNativeFactory()
                     ->GetD2D1DeviceContext()),
      window_(window) {
  window->GetWindowRenderTarget()->SetAsTarget();
  window->GetWindowRenderTarget()
      ->GetWinNativeFactory()
      ->GetD2D1DeviceContext()
      ->BeginDraw();
}

WindowPainter::~WindowPainter() { End(); }

void WindowPainter::DoEndDraw() {
  ThrowIfFailed(window_->GetWindowRenderTarget()
                    ->GetWinNativeFactory()
                    ->GetD2D1DeviceContext()
                    ->EndDraw());
  window_->GetWindowRenderTarget()->Present();
}
}  // namespace cru::win::native
