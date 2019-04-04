#pragma once
#include "win_pre_config.hpp"

#include "../graph_factory.hpp"

namespace cru::platform::win {
class GraphManager;

class WinGraphFactory : public Object, public virtual GraphFactory {
 public:
  explicit WinGraphFactory(GraphManager* graph_manager);
  WinGraphFactory(const WinGraphFactory& other) = delete;
  WinGraphFactory(WinGraphFactory&& other) = delete;
  WinGraphFactory& operator=(const WinGraphFactory& other) = delete;
  WinGraphFactory& operator=(WinGraphFactory&& other) = delete;
  ~WinGraphFactory() override = default;

  SolidColorBrush* CreateSolidColorBrush(const ui::Color& color) override;
  GeometryBuilder* CreateGeometryBuilder() override;
  FontDescriptor* CreateFontDescriptor(const std::wstring_view& font_family,
                                       float font_size);
  TextLayout* CreateTextLayout(std::shared_ptr<FontDescriptor> font,
                               std::wstring text);

 private:
  GraphManager* graph_manager_;
};
}  // namespace cru::platform::win
