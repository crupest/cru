#include "FocusConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
FocusConditionEditor::FocusConditionEditor() {
  SetLabel(u"Focus Condition");
  GetContainer()->AddChild(focus_check_box_.GetRootControl());

  focus_check_box_.SetLabel(u"Focus");
  focus_check_box_.ChangeEvent()->AddSpyOnlyHandler(
      [this] { change_event_.Raise(nullptr); });
}

FocusConditionEditor::~FocusConditionEditor() {}

ClonablePtr<ui::style::FocusCondition> FocusConditionEditor::GetValue() const {
  return ui::style::FocusCondition::Create(focus_check_box_.GetValue());
}

void FocusConditionEditor::SetValue(ui::style::FocusCondition* value,
                                    bool trigger_change) {
  focus_check_box_.SetValue(value->IsHasFocus(), trigger_change);
}
}  // namespace cru::theme_builder::components::conditions
