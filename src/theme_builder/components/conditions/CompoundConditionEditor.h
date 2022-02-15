#pragma once
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/components/Component.h"
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
  void SetChildren(std::vector<ClonablePtr<ui::style::Condition>> children);

  IEvent<std::nullptr_t>* ChildrenChangeEvent() {
    return &children_change_event_;
  }

 private:
  ui::controls::FlexLayout children_container_;
  ui::controls::Button add_child_button_;
  ui::controls::TextBlock add_child_button_text_;
  std::vector<std::unique_ptr<CompoundConditionEditorChild>> children_;

  Event<std::nullptr_t> children_change_event_;
};
}  // namespace cru::theme_builder::components::conditions
