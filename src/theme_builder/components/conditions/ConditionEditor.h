#pragma once
#include "../HeadBodyEditor.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class ConditionEditor : public HeadBodyEditor {
 public:
  ConditionEditor();
  ~ConditionEditor() override;

 public:
  virtual ClonablePtr<ui::style::Condition> GetCondition() = 0;
};

std::unique_ptr<ConditionEditor> CreateConditionEditor(
    ui::style::Condition* condition);
}  // namespace cru::theme_builder::components::conditions
