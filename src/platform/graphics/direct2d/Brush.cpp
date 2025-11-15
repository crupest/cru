#include "cru/platform/graphics/direct2d/Brush.h"
#include "cru/platform/graphics/direct2d/Factory.h"

namespace cru::platform::graphics::direct2d {
D2DSolidColorBrush::D2DSolidColorBrush(DirectGraphicsFactory* factory)
    : DirectGraphicsResource(factory) {
  CheckHResult(factory->GetDefaultD2D1DeviceContext()->CreateSolidColorBrush(
      Convert(color_), &brush_));
}

void D2DSolidColorBrush::SetColor(const Color& color) {
  brush_->SetColor(Convert(color));
}
}  // namespace cru::platform::graphics::direct2d
