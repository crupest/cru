#include "cru/ui/controls/Button.h"

#include "../Helper.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/render/BorderRenderObject.h"

namespace cru::ui::controls {
Button::Button() : click_detector_(this) {
  GetContainerRenderObject()->SetBorderEnabled(true);
  auto default_button_style =
      ThemeManager::GetInstance()->GetResourceStyleRuleSet(u"button.style");
  GetStyleRuleSet()->SetParent(std::move(default_button_style));
}

Button::~Button() = default;

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  GetContainerRenderObject()->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
