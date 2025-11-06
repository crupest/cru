#pragma once
#include "SingleChildControl.h"

#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "IBorderControl.h"
#include "IClickableControl.h"
#include "cru/base/Event.h"

namespace cru::ui::controls {
class CRU_UI_API Button : public SingleChildControl<render::BorderRenderObject>,
                          public virtual IClickableControl,
                          public virtual IBorderControl {
 public:
  static constexpr std::string_view kControlType = "Button";

 public:
  Button();
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button() override;

  std::string GetControlType() const final { return std::string(kControlType); }

 public:
  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  IEvent<const helper::ClickEventArgs&>* ClickEvent() {
    return click_detector_.ClickEvent();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
