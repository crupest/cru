#pragma once
#include "../ContentControl.hpp"
#include "Base.hpp"

#include "../ClickDetector.hpp"

namespace cru::ui::controls {
class Button : public ContentControl {
 public:
  static constexpr std::u16string_view control_type = u"Button";

  static Button* Create() { return new Button(); }

 protected:
  Button();

 public:
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button() override;

  std::u16string_view GetControlType() const final { return control_type; }

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
