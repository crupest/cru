#include "cru/ui/controls/Button.hpp"

#include "../Helper.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/render/BorderRenderObject.hpp"

namespace cru::ui::controls {
Button::Button() : click_detector_(this) {
  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
  render_object_->SetBorderEnabled(true);

  auto default_button_style =
      UiManager::GetInstance()->GetThemeResources()->button_style.get();
  GetStyleRuleSet()->SetParent(default_button_style);
}

Button::~Button() = default;

render::RenderObject* Button::GetRenderObject() const {
  return render_object_.get();
}

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  render_object_->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
