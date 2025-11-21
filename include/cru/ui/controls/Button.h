#pragma once
#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "IBorderControl.h"
#include "IClickableControl.h"
#include "SingleChildControl.h"

namespace cru::ui::controls {
class CRU_UI_API Button : public SingleChildControl<render::BorderRenderObject>,
                          public virtual IClickableControl,
                          public virtual IBorderControl {
 public:
  static constexpr auto kControlName = "Button";

 public:
  Button();
  ~Button() override;

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

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
