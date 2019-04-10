#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/brush.hpp"

namespace cru::win::graph {
class GraphManager;

struct WinBrush : virtual platform::graph::Brush {
  virtual ID2D1Brush* GetD2DBrush() = 0;
};

class WinSolidColorBrush : public Object,
                           public virtual platform::graph::SolidColorBrush,
                           public virtual WinBrush {
 public:
  explicit WinSolidColorBrush(GraphManager* graph_manager,
                              const ui::Color& color);
  WinSolidColorBrush(const WinSolidColorBrush& other) = delete;
  WinSolidColorBrush(WinSolidColorBrush&& other) = delete;
  WinSolidColorBrush& operator=(const WinSolidColorBrush& other) = delete;
  WinSolidColorBrush& operator=(WinSolidColorBrush&& other) = delete;
  ~WinSolidColorBrush() override = default;

  ui::Color GetColor() override;
  void SetColor(const ui::Color& color) override;

  ID2D1Brush* GetD2DBrush() override { return brush_.Get(); }

 private:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
};
}  // namespace cru::win::graph
