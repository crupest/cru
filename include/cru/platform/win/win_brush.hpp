#pragma once
#include "win_pre_config.hpp"

#include "../brush.hpp"

namespace cru::platform::win {
class WinSolidColorBrush : public Object, public virtual SolidColorBrush {
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

 private:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
};
}  // namespace cru::platform::win
