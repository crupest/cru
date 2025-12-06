#pragma once
#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "Control.h"
#include "IBorderControl.h"
#include "ICheckableControl.h"
#include "IClickableControl.h"

namespace cru::ui::controls {
class CRU_UI_API CheckBox : public Control,
                            public virtual IBorderControl,
                            public virtual ICheckableControl,
                            public virtual IClickableControl {
 public:
  static constexpr auto kControlName = "CheckBox";

  CheckBox();

  render::RenderObject* GetRenderObject() override {
    return &container_render_object_;
  }

  bool IsChecked() override { return checked_; }
  void SetChecked(bool checked) override;
  void Toggle() { SetChecked(!checked_); }

  IEvent<bool>* CheckedChangeEvent() override { return &checked_change_event_; }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  IEvent<const helper::ClickEventArgs&>* ClickEvent() override {
    return click_detector_.ClickEvent();
  }

 private:
  bool checked_;
  Event<bool> checked_change_event_;

  render::BorderRenderObject container_render_object_;

  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
