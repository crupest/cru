#pragma once
#include "ConditionEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/components/PopupButton.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class CompoundConditionEditor : public ConditionEditor {
 public:
  CompoundConditionEditor();
  ~CompoundConditionEditor();

 protected:
  std::vector<ClonePtr<ui::style::Condition>> GetChildren();
  void SetChildren(std::vector<ClonePtr<ui::style::Condition>> children,
                   bool trigger_change = true);

 private:
  ui::components::PopupMenuIconButton add_child_button_;
  ui::controls::FlexLayout children_container_;
  std::vector<platform::gui::DeleteLaterPtr<ConditionEditor>> children_;
};

class AndConditionEditor : public CompoundConditionEditor {
 public:
  ClonePtr<ui::style::AndCondition> GetValue() {
    return ui::style::AndCondition::Create(GetChildren());
  }
  void SetValue(ui::style::AndCondition* value, bool trigger_change = true) {
    SetChildren(value->GetChildren(), trigger_change);
  }
  void SetValue(const ClonePtr<ui::style::AndCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }
};

class OrConditionEditor : public CompoundConditionEditor {
 public:
  ClonePtr<ui::style::OrCondition> GetValue() {
    return ui::style::OrCondition::Create(GetChildren());
  }
  void SetValue(ui::style::OrCondition* value, bool trigger_change = true) {
    SetChildren(value->GetChildren(), trigger_change);
  }
  void SetValue(const ClonePtr<ui::style::OrCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }
};
}  // namespace cru::theme_builder::components::conditions
