#pragma once
#include "ConditionEditor.h"
#include "cru/base/ClonablePtr.h"
#include "cru/base/Event.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/components/PopupButton.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class CompoundConditionEditor : public ConditionEditor {
 public:
  CompoundConditionEditor();
  ~CompoundConditionEditor();

 protected:
  std::vector<ClonablePtr<ui::style::Condition>> GetChildren();
  void SetChildren(std::vector<ClonablePtr<ui::style::Condition>> children,
                   bool trigger_change = true);

 private:
  ui::components::PopupMenuIconButton add_child_button_;
  ui::controls::FlexLayout children_container_;
  std::vector<ui::DeleteLaterPtr<ConditionEditor>> children_;
};

class AndConditionEditor : public CompoundConditionEditor {
 public:
  AndConditionEditor() = default;
  ~AndConditionEditor() override = default;

 public:
  ClonablePtr<ui::style::AndCondition> GetValue() {
    return ui::style::AndCondition::Create(GetChildren());
  }
  void SetValue(ui::style::AndCondition* value, bool trigger_change = true) {
    SetChildren(value->GetChildren(), trigger_change);
  }
  void SetValue(const ClonablePtr<ui::style::AndCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonablePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }
};

class OrConditionEditor : public CompoundConditionEditor {
 public:
  OrConditionEditor() = default;
  ~OrConditionEditor() override = default;

 public:
  ClonablePtr<ui::style::OrCondition> GetValue() {
    return ui::style::OrCondition::Create(GetChildren());
  }
  void SetValue(ui::style::OrCondition* value, bool trigger_change = true) {
    SetChildren(value->GetChildren(), trigger_change);
  }
  void SetValue(const ClonablePtr<ui::style::OrCondition>& value,
                bool trigger_change = true) {
    SetValue(value.get(), trigger_change);
  }

  ClonablePtr<ui::style::Condition> GetCondition() override {
    return GetValue();
  }
};
}  // namespace cru::theme_builder::components::conditions
