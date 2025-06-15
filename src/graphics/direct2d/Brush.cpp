#include "cru/graphics/direct2d/Brush.h"

#include "cru/graphics/direct2d/ConvertUtil.h"
#include "cru/graphics/direct2d/Exception.h"
#include "cru/graphics/direct2d/Factory.h"

namespace cru::graphics::direct2d {
D2DSolidColorBrush::D2DSolidColorBrush(DirectGraphicsFactory* factory)
    : DirectGraphicsResource(factory) {
  ThrowIfFailed(factory->GetDefaultD2D1DeviceContext()->CreateSolidColorBrush(
      Convert(color_), &brush_));
}

void D2DSolidColorBrush::SetColor(const Color& color) {
  brush_->SetColor(Convert(color));
}
}  // namespace cru::graphics::direct2d
