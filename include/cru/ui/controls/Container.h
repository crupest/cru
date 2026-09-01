#pragma once
#include "../render/BorderRenderObject.h"
#include "Control.h"
#include "IBorderControl.h"

namespace cru::ui::controls {
class CRU_UI_API Container : public Control,
                             public Control::SingleChildMixin<Container>,
                             public virtual IBorderControl {
  static constexpr auto kControlName = "Container";

 public:
  Container();

 public:
  bool IsBorderEnabled() { return border_render_object_.IsBorderEnabled(); }
  void SetBorderEnabled(bool enabled) {
    border_render_object_.SetBorderEnabled(enabled);
  }

  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() {
    return border_render_object_.GetForegroundBrush();
  }
  void SetForegroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    border_render_object_.SetForegroundBrush(brush);
  }

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() {
    return border_render_object_.GetBackgroundBrush();
  }
  void SetBackgroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    border_render_object_.SetBackgroundBrush(brush);
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override {
    border_render_object_.ApplyBorderStyle(style);
  }

 private:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  render::BorderRenderObject border_render_object_;
};
}  // namespace cru::ui::controls
