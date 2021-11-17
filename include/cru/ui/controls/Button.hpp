#pragma once
#include "ContentControl.hpp"

#include "../helper/ClickDetector.hpp"
#include "IBorderControl.hpp"
#include "IClickableControl.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"

namespace cru::ui::controls {
class Button : public ContentControl,
               public virtual IClickableControl,
               public virtual IBorderControl {
 public:
  static constexpr StringView control_type = u"Button";

  static Button* Create() { return new Button(); }

 protected:
  Button();

 public:
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button() override;

  String GetControlType() const final { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

 public:
  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_{};

  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
