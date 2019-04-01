#include "cru/platform/win/win_brush.hpp"

#include "cru/platform/win/d2d_util.hpp"
#include "cru/platform/win/exception.hpp"

#include <cassert>

namespace cru::platform::win {
WinSolidColorBrush::WinSolidColorBrush(
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush) {
  assert(brush);
  brush_ = std::move(brush);
}

ui::Color cru::platform::win::WinSolidColorBrush::GetColor() {
  return util::Convert(brush_->GetColor());
}
void WinSolidColorBrush::SetColor(const ui::Color& color) {
  brush_->SetColor(util::Convert(color));
}
}  // namespace cru::platform::win
