#pragma once
#include "ConditionEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class NoConditionEditor : public ConditionEditor {
 public:
  NoConditionEditor();
  ~NoConditionEditor() override;

 public:
  ClonePtr<ui::style::Condition> GetCondition() override {
    return ui::style::NoCondition::Create();
  }
};
}  // namespace cru::theme_builder::components::conditions
