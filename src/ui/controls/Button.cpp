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
  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
  render_object_->SetBorderEnabled(true);

  auto default_button_style =
      ThemeManager::GetInstance()->GetResourceStyleRuleSet(u"button.style");
  GetStyleRuleSet()->SetParent(std::move(default_button_style));
}

Button::~Button() = default;

render::RenderObject* Button::GetRenderObject() const {
  return render_object_.get();
}

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  render_object_->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
