#include "CheckBoxPropertyEditor.h"

namespace cru::theme_builder::components::properties {
CheckBoxPropertyEditor::CheckBoxPropertyEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&label_);
  container_.AddChild(&check_box_);

  check_box_.CheckedChangeEvent()->AddSpyOnlyHandler([this] {
    if (!suppress_next_change_event_) {
      change_event_.Raise(nullptr);
    } else {
      suppress_next_change_event_ = false;
    }
  });
}

CheckBoxPropertyEditor::~CheckBoxPropertyEditor() {
  container_.RemoveFromParent();
}

void CheckBoxPropertyEditor::SetValue(bool value, bool trigger_change) {
  if (!trigger_change) suppress_next_change_event_ = true;
  check_box_.SetChecked(value);
}
}  // namespace cru::theme_builder::components::properties
