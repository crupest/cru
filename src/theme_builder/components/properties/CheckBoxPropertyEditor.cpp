#include "CheckBoxPropertyEditor.h"

namespace cru::theme_builder::components::properties {
CheckBoxPropertyEditor::CheckBoxPropertyEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&label_);
  container_.AddChild(&check_box_);

  check_box_.CheckedChangeEvent()->AddSpyOnlyHandler(
      [this] { RaiseChangeEvent(); });
}

CheckBoxPropertyEditor::~CheckBoxPropertyEditor() {}

void CheckBoxPropertyEditor::SetValue(bool value, bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  check_box_.SetChecked(value);
}
}  // namespace cru::theme_builder::components::properties
