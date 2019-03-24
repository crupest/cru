#pragma once
#include "pre.hpp"

#include "ui/content_control.hpp"

namespace cru::ui::render {
class BorderRenderObject;
}

namespace cru::ui::controls {
class Button : public ContentControl {
 public:
  static constexpr auto control_type = L"Button";

  static Button* Create();

 protected:
  Button();

 public:
  Button(const Button& other) = delete;
  Button(Button&& other) = delete;
  Button& operator=(const Button& other) = delete;
  Button& operator=(Button&& other) = delete;
  ~Button();

  render::RenderObject* GetRenderObject() const override;

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  render::BorderRenderObject* render_object_;
};
}  // namespace cru::ui::controls
