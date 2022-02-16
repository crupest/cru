#pragma once
#include "../properties/CheckBoxPropertyEditor.h"
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class CheckedConditionEditor : public ConditionEditor {
 public:
  CheckedConditionEditor();
  ~CheckedConditionEditor() override;

 public:
  ClonablePtr<ui::style::CheckedCondition> GetValue() const;
  void SetValue(ui::style::CheckedCondition* value, bool trigger_change = true);
  void SetValue(const ClonablePtr<ui::style::CheckedCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonablePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

  IEvent<std::nullptr_t>* ChangeEvent() override { return &change_event_; }

 private:
  properties::CheckBoxPropertyEditor checked_check_box_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::conditions
