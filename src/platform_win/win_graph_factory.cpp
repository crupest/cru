#include "cru/platform/win/win_graph_factory.hpp"

#include "cru/platform/win/d2d_util.hpp"
#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/graph_manager.hpp"
#include "cru/platform/win/win_brush.hpp"
#include "cru/platform/win/win_font.hpp"
#include "cru/platform/win/win_geometry.hpp"
#include "cru/platform/win/win_text_layout.hpp"

#include <cassert>
#include <utility>

namespace cru::platform::win {
WinGraphFactory::WinGraphFactory(GraphManager* graph_manager) {
  assert(graph_manager);
  graph_manager_ = graph_manager;
}

SolidColorBrush* WinGraphFactory::CreateSolidColorBrush(
    const ui::Color& color) {
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
  ThrowIfFailed(graph_manager_->GetD2D1DeviceContext()->CreateSolidColorBrush(
      util::Convert(color), &brush));
  return new WinSolidColorBrush(std::move(brush));
}

GeometryBuilder* WinGraphFactory::CreateGeometryBuilder() {
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
  ThrowIfFailed(
      graph_manager_->GetD2D1Factory()->CreatePathGeometry(&geometry));
  return new WinGeometryBuilder(std::move(geometry));
}

FontDescriptor* WinGraphFactory::CreateFontDescriptor(
    const std::wstring_view& font_family, float font_size) {
  return new WinFontDescriptor(graph_manager_, font_family, font_size);
}

TextLayout* WinGraphFactory::CreateTextLayout(
    std::shared_ptr<FontDescriptor> font, std::wstring text) {
  const auto f = std::dynamic_pointer_cast<WinFontDescriptor>(font);
  assert(f);
  return new WinTextLayout(graph_manager_, std::move(f), std::move(text));
}
}  // namespace cru::platform::win
