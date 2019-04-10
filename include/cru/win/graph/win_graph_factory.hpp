#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/graph_factory.hpp"

namespace cru::win::graph {
class GraphManager;

class WinGraphFactory : public Object,
                        public virtual platform::graph::GraphFactory {
 public:
  explicit WinGraphFactory(GraphManager* graph_manager);
  WinGraphFactory(const WinGraphFactory& other) = delete;
  WinGraphFactory(WinGraphFactory&& other) = delete;
  WinGraphFactory& operator=(const WinGraphFactory& other) = delete;
  WinGraphFactory& operator=(WinGraphFactory&& other) = delete;
  ~WinGraphFactory() override = default;

  platform::graph::SolidColorBrush* CreateSolidColorBrush(
      const ui::Color& color) override;
  platform::graph::GeometryBuilder* CreateGeometryBuilder() override;
  platform::graph::FontDescriptor* CreateFontDescriptor(
      const std::wstring_view& font_family, float font_size);
  platform::graph::TextLayout* CreateTextLayout(
      std::shared_ptr<platform::graph::FontDescriptor> font, std::wstring text);

 private:
  GraphManager* graph_manager_;
};
}  // namespace cru::win::graph
