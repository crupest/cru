#include "cru/win/graphics/direct/Brush.hpp"

#include "cru/win/graphics/direct/ConvertUtil.hpp"
#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"

namespace cru::platform::graphics::win::direct {
D2DSolidColorBrush::D2DSolidColorBrush(DirectGraphicsFactory* factory)
    : DirectGraphicsResource(factory) {
  ThrowIfFailed(factory->GetDefaultD2D1DeviceContext()->CreateSolidColorBrush(
      Convert(color_), &brush_));
}

void D2DSolidColorBrush::SetColor(const Color& color) {
  brush_->SetColor(Convert(color));
}
}  // namespace cru::platform::graphics::win::direct
