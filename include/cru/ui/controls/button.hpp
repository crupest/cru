#pragma once
#include "../content_control.hpp"

#include <memory>

namespace cru::ui::render {
class BorderRenderObject;
}

namespace cru::ui::controls {
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

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  std::shared_ptr<render::BorderRenderObject> render_object_{};
};
}  // namespace cru::ui::controls
