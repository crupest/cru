#include "cru/win/graph/win_brush.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/d2d_util.hpp"
#include "cru/win/graph/graph_manager.hpp"

#include <cassert>

namespace cru::win::graph {
WinSolidColorBrush::WinSolidColorBrush(GraphManager* graph_manager,
                                       const ui::Color& color) {
  assert(graph_manager);
  ThrowIfFailed(graph_manager->GetD2D1DeviceContext()->CreateSolidColorBrush(
      util::Convert(color), &brush_));
}

ui::Color WinSolidColorBrush::GetColor() {
  return util::Convert(brush_->GetColor());
}
void WinSolidColorBrush::SetColor(const ui::Color& color) {
  brush_->SetColor(util::Convert(color));
}
}  // namespace cru::win::graph
