#pragma once
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/components/PopupButton.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class CompoundConditionEditorChild : public ui::components::Component {
 public:
  explicit CompoundConditionEditorChild(
      std::unique_ptr<ConditionEditor>&& editor);
  ~CompoundConditionEditorChild() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ConditionEditor* GetConditionEditor() { return condition_editor_.get(); }

  IEvent<std::nullptr_t>* RemoveEvent() { return &remove_event_; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::Button remove_button_;
  ui::controls::TextBlock remove_button_text_;
  std::unique_ptr<ConditionEditor> condition_editor_;

  Event<std::nullptr_t> remove_event_;
};

class CompoundConditionEditor : public ConditionEditor {
 public:
  CompoundConditionEditor();
  ~CompoundConditionEditor();

 protected:
  std::vector<ClonablePtr<ui::style::Condition>> GetChildren();
  void SetChildren(std::vector<ClonablePtr<ui::style::Condition>> children,
                   bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() override { return &change_event_; }

 private:
  ui::controls::FlexLayout children_container_;
  ui::components::PopupMenuTextButton add_child_button_;
  std::vector<std::unique_ptr<CompoundConditionEditorChild>> children_;

  Event<std::nullptr_t> change_event_;
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
