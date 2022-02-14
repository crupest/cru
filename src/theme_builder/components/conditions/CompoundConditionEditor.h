#pragma once
#include "ConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
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

  Event<std::nullptr_t> children_change_event_;
};
}  // namespace cru::theme_builder::components::conditions
