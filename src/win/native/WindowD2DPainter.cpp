#include "WindowD2DPainter.hpp"

#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Factory.hpp"
#include "cru/win/native/WindowRenderTarget.hpp"

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
