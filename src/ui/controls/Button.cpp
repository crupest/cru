#include "cru/ui/controls/Button.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/helper/ClickDetector.h"

namespace cru::ui::controls {
Button::Button()
    : Control(kControlName, &border_render_object_), click_detector_(this) {
  border_render_object_.SetAttachedControl(this);
  border_render_object_.SetBorderEnabled(true);
  auto default_button_style =
      ThemeManager::GetInstance()->GetResourceStyleRuleSet("button.style");
  GetStyleRuleSet()->SetParent(std::move(default_button_style));
}

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  border_render_object_.ApplyBorderStyle(style);
}

void Button::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) {
    border_render_object_.SetChild(nullptr);
  }
  if (new_child) {
    border_render_object_.SetChild(new_child->GetRenderObject());
  }
}

}  // namespace cru::ui::controls
