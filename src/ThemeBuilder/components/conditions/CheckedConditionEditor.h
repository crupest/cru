#pragma once
#include "../properties/CheckBoxPropertyEditor.h"
#include "ConditionEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class CheckedConditionEditor : public ConditionEditor {
 public:
  CheckedConditionEditor();
  ~CheckedConditionEditor() override;

 public:
  ClonePtr<ui::style::CheckedCondition> GetValue() const;
  void SetValue(ui::style::CheckedCondition* value, bool trigger_change = true);
  void SetValue(const ClonePtr<ui::style::CheckedCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

 private:
  properties::CheckBoxPropertyEditor checked_check_box_;
};
}  // namespace cru::theme_builder::components::conditions
