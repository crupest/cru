#include "cru/win/graph/win_graph_factory.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/d2d_util.hpp"
#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/graph/win_brush.hpp"
#include "cru/win/graph/win_font.hpp"
#include "cru/win/graph/win_geometry.hpp"
#include "cru/win/graph/win_text_layout.hpp"

#include <cassert>
#include <utility>

namespace cru::platform::graph {
GraphFactory* GraphFactory::GetInstance() {
  return win::graph::GraphManager::GetInstance()->GetGraphFactory();
}
}

namespace cru::win::graph {
WinGraphFactory::WinGraphFactory(GraphManager* graph_manager) {
  assert(graph_manager);
  graph_manager_ = graph_manager;
}

platform::graph::SolidColorBrush* WinGraphFactory::CreateSolidColorBrush(
    const ui::Color& color) {
  return new WinSolidColorBrush(graph_manager_, color);
}

platform::graph::GeometryBuilder* WinGraphFactory::CreateGeometryBuilder() {
  return new WinGeometryBuilder(graph_manager_);
}

platform::graph::FontDescriptor* WinGraphFactory::CreateFontDescriptor(
    const std::wstring_view& font_family, float font_size) {
  return new WinFontDescriptor(graph_manager_, font_family, font_size);
}

platform::graph::TextLayout* WinGraphFactory::CreateTextLayout(
    std::shared_ptr<platform::graph::FontDescriptor> font, std::wstring text) {
  const auto f = std::dynamic_pointer_cast<WinFontDescriptor>(font);
  assert(f);
  return new WinTextLayout(graph_manager_, std::move(f), std::move(text));
}
}  // namespace cru::win::graph
