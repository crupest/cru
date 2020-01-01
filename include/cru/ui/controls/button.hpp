#pragma once
#include "../content_control.hpp"

#include "../click_detector.hpp"
#include "../render/border_render_object.hpp"
#include "cru/platform/native/basic_types.hpp"

#include <memory>

namespace cru::ui::controls {
using render::CornerRadius;

struct ButtonStateStyle {
  std::shared_ptr<platform::graph::IBrush> border_brush;
  Thickness border_thickness;
  CornerRadius border_radius;
  std::shared_ptr<platform::graph::IBrush> foreground_brush;
  std::shared_ptr<platform::graph::IBrush> background_brush;
};

struct ButtonStyle {
  // corresponds to ClickState::None
  ButtonStateStyle normal;
  // corresponds to ClickState::Hover
  ButtonStateStyle hover;
  // corresponds to ClickState::Press
  ButtonStateStyle press;
  // corresponds to ClickState::PressInactive
  ButtonStateStyle press_cancel;
};

class Button : public ContentControl {
 public:
  static constexpr std::string_view control_type = "Button";

  static Button* Create() { return new Button(); }

 protected:
  Button();

 public:
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button() override = default;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 public:
  const ButtonStyle& GetStyle() const { return style_; }
  void SetStyle(ButtonStyle style);

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_{};

  ButtonStyle style_;

  ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
