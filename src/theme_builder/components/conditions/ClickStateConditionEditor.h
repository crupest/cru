#pragma once
#include "../properties/SelectPropertyEditor.h"
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class ClickStateConditionEditor : public ConditionEditor {
 public:
  ClickStateConditionEditor();
  ~ClickStateConditionEditor();

 public:
  ClonablePtr<ui::style::ClickStateCondition> GetValue() const;
  void SetValue(ClonablePtr<ui::style::ClickStateCondition> value,
                bool trigger_change = true);

  ClonablePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  properties::SelectPropertyEditor click_state_select_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::conditions
