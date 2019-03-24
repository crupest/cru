#include "button.hpp"

#include "ui/ui_manager.hpp"
#include "ui/render/border_render_object.hpp"

namespace cru::ui::controls {
Button::Button() {
  const auto predefined_resource =
      UiManager::GetInstance()->GetPredefineResources();
  render_object_ = new render::BorderRenderObject(); }

void Button::OnChildChanged(Control* old_child, Control* new_child) {}
}  // namespace cru::ui::controls
