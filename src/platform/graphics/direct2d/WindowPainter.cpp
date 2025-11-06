#include "cru/platform/graphics/direct2d/WindowPainter.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/graphics/direct2d/WindowRenderTarget.h"

namespace cru::platform::graphics::direct2d {
D2DWindowPainter::D2DWindowPainter(DirectGraphicsFactory* graphics_factory,
                                   D2DWindowRenderTarget* render_target)
    : D2DDeviceContextPainter(graphics_factory,
                              render_target->GetD2D1DeviceContext(), false),
      render_target_(render_target) {}

D2DWindowPainter::~D2DWindowPainter() {}

void D2DWindowPainter::DoEndDraw() { render_target_->Present(); }
}  // namespace cru::platform::graphics::direct2d
