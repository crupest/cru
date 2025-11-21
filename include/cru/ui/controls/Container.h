#pragma once
#include "../render/BorderRenderObject.h"
#include "IBorderControl.h"
#include "SingleChildControl.h"

namespace cru::ui::controls {
class CRU_UI_API Container
    : public SingleChildControl<render::BorderRenderObject>,
      public virtual IBorderControl {
  static constexpr auto kControlName = "Container";

 public:
  Container();

 public:
  bool IsBorderEnabled() {
    return GetContainerRenderObject()->IsBorderEnabled();
  }
  void SetBorderEnabled(bool enabled) {
    GetContainerRenderObject()->SetBorderEnabled(enabled);
  }

  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() {
    return GetContainerRenderObject()->GetForegroundBrush();
  }
  void SetForegroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetForegroundBrush(brush);
  }

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() {
    return GetContainerRenderObject()->GetBackgroundBrush();
  }
  void SetBackgroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetBackgroundBrush(brush);
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override {
    GetContainerRenderObject()->ApplyBorderStyle(style);
  }
};
}  // namespace cru::ui::controls
