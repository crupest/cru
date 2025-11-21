#include "FocusConditionEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
FocusConditionEditor::FocusConditionEditor() {
  SetLabel("Focus Condition");
  GetContainer()->AddChild(focus_check_box_.GetRootControl());

  focus_check_box_.SetLabel("Focus");

  ConnectChangeEvent(focus_check_box_);
}

FocusConditionEditor::~FocusConditionEditor() {}

ClonePtr<ui::style::FocusCondition> FocusConditionEditor::GetValue() {
  return ui::style::FocusCondition::Create(focus_check_box_.GetValue());
}

void FocusConditionEditor::SetValue(ui::style::FocusCondition* value,
                                    bool trigger_change) {
  focus_check_box_.SetValue(value->IsHasFocus(), trigger_change);
}
}  // namespace cru::theme_builder::components::conditions
