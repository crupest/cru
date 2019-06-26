#include "cru/win/graph/direct/brush.hpp"

#include "cru/win/graph/direct/convert_util.hpp"
#include "cru/win/graph/direct/direct_factory.hpp"
#include "cru/win/graph/direct/exception.hpp"

#include <cassert>

namespace cru::platform::graph::win::direct {
D2DSolidColorBrush::D2DSolidColorBrush(IDirectFactory* factory) {
  assert(factory);
  ThrowIfFailed(factory->GetD2D1DeviceContext()->CreateSolidColorBrush(
      Convert(color_), &brush_));
}

void D2DSolidColorBrush::OnSetColor(const Color& color) {
  brush_->SetColor(Convert(color));
}
}  // namespace cru::platform::graph::win::direct
