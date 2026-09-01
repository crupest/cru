#pragma once
#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "Control.h"
#include "IBorderControl.h"
#include "IClickableControl.h"

namespace cru::ui::controls {
class CRU_UI_API Button : public Control,
                          public Control::SingleChildMixin<Button>,
                          public virtual IClickableControl,
                          public virtual IBorderControl {
 public:
  static constexpr auto kControlName = "Button";

 public:
  Button();

 public:
  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  IEvent<const helper::ClickEventArgs&>* ClickEvent() override {
    return click_detector_.ClickEvent();
  }

  IEvent<const helper::ClickEventArgs&>* DoubleClickEvent() override {
    return click_detector_.DoubleClickEvent();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  helper::ClickDetector click_detector_;
  render::BorderRenderObject border_render_object_;
};
}  // namespace cru::ui::controls
