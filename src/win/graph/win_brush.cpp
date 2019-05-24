#include "cru/win/graph/win_brush.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/win_native_factory.hpp"
#include "cru/win/graph/util/convert_util.hpp"

#include <cassert>

namespace cru::win::graph {
WinSolidColorBrush::WinSolidColorBrush(IWinNativeFactory* factory,
                                       const ui::Color& color) {
  assert(factory);
  ThrowIfFailed(factory->GetD2D1DeviceContext()->CreateSolidColorBrush(
      util::Convert(color), &brush_));
}

ui::Color WinSolidColorBrush::GetColor() {
  return util::Convert(brush_->GetColor());
}
void WinSolidColorBrush::SetColor(const ui::Color& color) {
  brush_->SetColor(util::Convert(color));
}
}  // namespace cru::win::graph
