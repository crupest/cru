#pragma once
#include "SingleChildControl.h"

#include "../render/BorderRenderObject.h"
#include "IBorderControl.h"

namespace cru::ui::controls {
class CRU_UI_API Container
    : public SingleChildControl<render::BorderRenderObject>,
      public virtual IBorderControl {
  static constexpr std::string_view kControlType = "Container";

 public:
  Container();
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  bool IsBorderEnabled() const {
    return GetContainerRenderObject()->IsBorderEnabled();
  }
  void SetBorderEnabled(bool enabled) {
    GetContainerRenderObject()->SetBorderEnabled(enabled);
  }

  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() const {
    return GetContainerRenderObject()->GetForegroundBrush();
  }
  void SetForegroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetForegroundBrush(brush);
  }

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() const {
    return GetContainerRenderObject()->GetBackgroundBrush();
  }
  void SetBackgroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetBackgroundBrush(brush);
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override {
    GetContainerRenderObject()->ApplyBorderStyle(style);
  }

 public:
  std::string GetControlType() const final { return std::string(kControlType); }
};
}  // namespace cru::ui::controls
