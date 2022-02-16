#include "CheckedConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
CheckedConditionEditor::CheckedConditionEditor() {
  GetContainer()->AddChild(checked_check_box_.GetRootControl());

  checked_check_box_.ChangeEvent()->AddSpyOnlyHandler(
      [this] { change_event_.Raise(nullptr); });
}

CheckedConditionEditor::~CheckedConditionEditor() {}

ClonablePtr<ui::style::CheckedCondition> CheckedConditionEditor::GetValue()
    const {
  return ui::style::CheckedCondition::Create(checked_check_box_.GetValue());
}

void CheckedConditionEditor::SetValue(
    const ClonablePtr<ui::style::CheckedCondition> &value,
    bool trigger_change) {
  checked_check_box_.SetValue(value->IsChecked(), trigger_change);
}
}  // namespace cru::theme_builder::components::conditions
