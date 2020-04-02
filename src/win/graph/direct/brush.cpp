#include "cru/win/graph/direct/brush.hpp"

#include "cru/win/graph/direct/convert_util.hpp"
#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/factory.hpp"

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
