#include "window_d2d_painter.hpp"

#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/factory.hpp"
#include "cru/win/native/window_render_target.hpp"

#include <cassert>

namespace cru::platform::native::win {
using namespace cru::platform::graph::win::direct;

WindowD2DPainter::WindowD2DPainter(WindowRenderTarget* render_target)
    : D2DPainter(render_target->GetD2D1DeviceContext()),
      render_target_(render_target) {
  render_target_->GetD2D1DeviceContext()->BeginDraw();
}

WindowD2DPainter::~WindowD2DPainter() { EndDraw(); }

void WindowD2DPainter::DoEndDraw() {
  ThrowIfFailed(render_target_->GetD2D1DeviceContext()->EndDraw());
  render_target_->Present();
}
}  // namespace cru::platform::native::win
