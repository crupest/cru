#pragma once
#include "../properties/CheckBoxPropertyEditor.h"
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class FocusConditionEditor : public ConditionEditor {
 public:
  FocusConditionEditor();
  ~FocusConditionEditor() override;

 public:
  ClonablePtr<ui::style::FocusCondition> GetValue() const;
  void SetValue(ui::style::FocusCondition* value, bool trigger_change = true);
  void SetValue(const ClonablePtr<ui::style::FocusCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonablePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

 private:
  properties::CheckBoxPropertyEditor focus_check_box_;
};
}  // namespace cru::theme_builder::components::conditions
