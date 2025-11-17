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
  static constexpr std::string_view kControlType = "CheckBox";

  CheckBox();
  ~CheckBox() override;

  std::string GetControlType() const override {
    return std::string(kControlType);
  }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  bool IsChecked() const override { return checked_; }
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

 private:
  bool checked_ = false;
  Event<bool> checked_change_event_;

  std::unique_ptr<render::BorderRenderObject> container_render_object_;

  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
