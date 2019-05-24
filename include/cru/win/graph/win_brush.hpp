#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/brush.hpp"

namespace cru::win::graph {
struct IWinNativeFactory;

struct IWinBrush : virtual platform::graph::IBrush {
  virtual ID2D1Brush* GetD2DBrush() = 0;
};

class WinSolidColorBrush : public Object,
                           public virtual platform::graph::ISolidColorBrush,
                           public virtual IWinBrush {
 public:
  WinSolidColorBrush(IWinNativeFactory* factory, const ui::Color& color);
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
