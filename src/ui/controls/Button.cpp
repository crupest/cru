#include "cru/ui/controls/Button.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/render/BorderRenderObject.h"

namespace cru::ui::controls {
Button::Button()
    : SingleChildControl<render::BorderRenderObject>(kControlName),
      click_detector_(this) {
  GetContainerRenderObject()->SetBorderEnabled(true);
  auto default_button_style =
      ThemeManager::GetInstance()->GetResourceStyleRuleSet("button.style");
  GetStyleRuleSet()->SetParent(std::move(default_button_style));
}

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  GetContainerRenderObject()->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
