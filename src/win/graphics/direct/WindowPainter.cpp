#include "cru/win/graphics/direct/WindowPainter.h"

#include "cru/win/graphics/direct/Exception.h"
#include "cru/win/graphics/direct/Factory.h"
#include "cru/win/graphics/direct/WindowRenderTarget.h"

namespace cru::platform::graphics::win::direct {
D2DWindowPainter::D2DWindowPainter(D2DWindowRenderTarget* render_target)
    : D2DDeviceContextPainter(render_target->GetD2D1DeviceContext()),
      render_target_(render_target) {}

D2DWindowPainter::~D2DWindowPainter() {}

void D2DWindowPainter::DoEndDraw() { render_target_->Present(); }
}  // namespace cru::platform::graphics::win::direct
