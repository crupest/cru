#include "CheckedConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
CheckedConditionEditor::CheckedConditionEditor() {
  SetLabel(u"Checked Condition");

  checked_check_box_.SetLabel(u"Checked");
  GetContainer()->AddChild(checked_check_box_.GetRootControl());

  ConnectChangeEvent(checked_check_box_);
}

CheckedConditionEditor::~CheckedConditionEditor() {}

ClonablePtr<ui::style::CheckedCondition> CheckedConditionEditor::GetValue()
    const {
  return ui::style::CheckedCondition::Create(checked_check_box_.GetValue());
}

void CheckedConditionEditor::SetValue(ui::style::CheckedCondition* value,
                                      bool trigger_change) {
  checked_check_box_.SetValue(value->IsChecked(), trigger_change);
}
}  // namespace cru::theme_builder::components::conditions
