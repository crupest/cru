#include "cru/win/graph/direct/Brush.hpp"

#include "cru/win/graph/direct/ConvertUtil.hpp"
#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Factory.hpp"

namespace cru::platform::graph::win::direct {
D2DSolidColorBrush::D2DSolidColorBrush(DirectGraphFactory* factory)
    : DirectGraphResource(factory) {
  ThrowIfFailed(factory->GetDefaultD2D1DeviceContext()->CreateSolidColorBrush(
      Convert(color_), &brush_));
}

void D2DSolidColorBrush::SetColor(const Color& color) {
  brush_->SetColor(Convert(color));
}
}  // namespace cru::platform::graph::win::direct
