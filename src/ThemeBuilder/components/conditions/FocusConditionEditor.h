#pragma once
#include "../properties/CheckBoxPropertyEditor.h"
#include "ConditionEditor.h"
#include <cru/CopyPtr.h>
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class FocusConditionEditor : public ConditionEditor {
 public:
  FocusConditionEditor();
  ~FocusConditionEditor() override;

 public:
  CopyPtr<ui::style::FocusCondition> GetValue() const;
  void SetValue(ui::style::FocusCondition* value, bool trigger_change = true);
  void SetValue(const CopyPtr<ui::style::FocusCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  CopyPtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

 private:
  properties::CheckBoxPropertyEditor focus_check_box_;
};
}  // namespace cru::theme_builder::components::conditions
