#include "window_d2d_painter.hpp"

#include "cru/win/graph/direct/direct_factory.hpp"
#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/native/window_render_target.hpp"

#include <cassert>

namespace cru::platform::native::win {
using namespace cru::platform::graph::win::direct;

WindowD2DPainter::WindowD2DPainter(WinNativeWindow* window)
    : D2DPainter(window->GetWindowRenderTarget()
                     ->GetWinNativeFactory()
                     ->GetD2D1DeviceContext()),
      window_(window) {
  window->GetWindowRenderTarget()->SetAsTarget();
  window->GetWindowRenderTarget()
      ->GetWinNativeFactory()
      ->GetD2D1DeviceContext()
      ->BeginDraw();
}

WindowD2DPainter::~WindowD2DPainter() { EndDraw(); }

void WindowD2DPainter::DoEndDraw() {
  ThrowIfFailed(window_->GetWindowRenderTarget()
                    ->GetWinNativeFactory()
                    ->GetD2D1DeviceContext()
                    ->EndDraw());
  window_->GetWindowRenderTarget()->Present();
}
}  // namespace cru::platform::native::win
