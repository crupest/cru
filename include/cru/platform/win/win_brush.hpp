#pragma once
#include "win_pre_config.hpp"

#include "../brush.hpp"

namespace cru::platform::win {
struct WinBrush : virtual Brush {
  virtual ID2D1Brush* GetD2DBrush() = 0;
};

class WinSolidColorBrush : public Object,
                           public virtual SolidColorBrush,
                           public virtual WinBrush {
 public:
  explicit WinSolidColorBrush(
      Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush);
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
}  // namespace cru::platform::win
