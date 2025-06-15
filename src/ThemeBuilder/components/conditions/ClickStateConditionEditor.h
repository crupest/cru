#pragma once
#include "../properties/SelectPropertyEditor.h"
#include "ConditionEditor.h"
#include <cru/CopyPtr.h>
#include <cru/Event.h>
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class ClickStateConditionEditor : public ConditionEditor {
 public:
  ClickStateConditionEditor();
  ~ClickStateConditionEditor();

 public:
  CopyPtr<ui::style::ClickStateCondition> GetValue() const;
  void SetValue(ui::style::ClickStateCondition* value,
                bool trigger_change = true);
  void SetValue(const CopyPtr<ui::style::ClickStateCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  CopyPtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

 private:
  properties::SelectPropertyEditor click_state_select_;
};
}  // namespace cru::theme_builder::components::conditions
