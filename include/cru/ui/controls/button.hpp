#pragma once
#include "../content_control.hpp"

#include "../click_detector.hpp"
#include "../render/border_render_object.hpp"
#include "cru/platform/graph/brush.hpp"
#include "cru/platform/native/basic_types.hpp"

#include <memory>

namespace cru::ui::render {
class BorderRenderObject;
}

namespace cru::ui::controls {
struct ButtonBorderStyle {
  // corresponds to ButtonState::Normal
  render::BorderStyle normal;
  // corresponds to ButtonState::Hover
  render::BorderStyle hover;
  // corresponds to ButtonState::Press
  render::BorderStyle press;
  // corresponds to ButtonState::PressCancel
  render::BorderStyle press_cancel;
};

enum class ButtonState {
  // mouse is not in it
  Normal,
  // mouse is in it and not pressed
  Hover,
  // mouse is pressed in it
  Press,
  // mouse is pressed outside button
  PressCancel,  
};

class Button : public ContentControl {
 public:
  static constexpr auto control_type = L"Button";

  static Button* Create() { return new Button(); }

 protected:
  Button();

 public:
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button() override = default;

  std::wstring_view GetControlType() const override final {
    return control_type;
  }

  render::RenderObject* GetRenderObject() const override;

 public:
  render::BorderStyle GetNormalBorderStyle() const {
    return border_style_.normal;
  }
  void SetNormalBorderStyle(render::BorderStyle newStyle) {
    border_style_.normal = std::move(newStyle);
  }

  render::BorderStyle GetHoverBorderStyle() const {
    return border_style_.hover;
  }
  void SetHoverBorderStyle(render::BorderStyle newStyle) {
    border_style_.hover = std::move(newStyle);
  }

  render::BorderStyle GetPressBorderStyle() const {
    return border_style_.press;
  }
  void SetPressBorderStyle(render::BorderStyle newStyle) {
    border_style_.press = std::move(newStyle);
  }

  ButtonBorderStyle GetBorderStyle() const { return border_style_; }
  void SetBorderStyle(ButtonBorderStyle newStyle) {
    border_style_ = std::move(newStyle);
  }

  ButtonState GetState() const { return state_; }

  // Get the trigger mouse button(s). Return value might be a union of Left,
  // Middle and Right.
  MouseButton GetTriggerButton() const { return trigger_button_; }
  // Set the trigger mouse button(s). You can set multiple buttons by passing a
  // union of Left, Middle and Right. If you disable a button when user is
  // pressing the same one, click will be stopped.
  // Default is only Left.
  void SetTriggerButton(MouseButton newTrigger);

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

  virtual void OnStateChange(ButtonState oldState, ButtonState newState);

 private:
  void SetState(ButtonState newState) {
    if (state_ == newState) return;
    const auto oldState = state_;
    state_ = newState;
    OnStateChange(oldState, newState);
  }

 private:
  std::shared_ptr<render::BorderRenderObject> render_object_{};

  ButtonState state_ = ButtonState::Normal;

  MouseButton trigger_button_ = MouseButton::Left;

  ButtonBorderStyle border_style_;

  ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
