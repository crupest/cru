#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/render/BorderRenderObject.h"

namespace cru::ui::controls {
CheckBox::CheckBox()
    : container_render_object_(new render::BorderRenderObject()),
      click_detector_(this) {
  container_render_object_->SetAttachedControl(this);

  click_detector_.ClickEvent()->AddHandler(
      [this](const helper::ClickEventArgs&) { Toggle(); });
}

CheckBox::~CheckBox() {}

void CheckBox::SetChecked(bool checked) {
  checked_ = checked;
  checked_change_event_.Raise(checked);
}

void CheckBox::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  container_render_object_->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
