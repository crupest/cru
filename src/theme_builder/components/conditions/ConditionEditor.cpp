#include "ConditionEditor.h"
#include "CheckedConditionEditor.h"
#include "ClickStateConditionEditor.h"
#include "CompoundConditionEditor.h"
#include "FocusConditionEditor.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::conditions {
ConditionEditor::ConditionEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(&label_);
}

ConditionEditor::~ConditionEditor() { container_.RemoveFromParent(); }

std::unique_ptr<ConditionEditor> CreateConditionEditor(
    ui::style::Condition* condition) {
  if (auto and_condition = dynamic_cast<ui::style::AndCondition*>(condition)) {
    auto result = std::make_unique<AndConditionEditor>();
    result->SetValue(and_condition);
    return result;
  } else if (auto or_condition =
                 dynamic_cast<ui::style::OrCondition*>(condition)) {
    auto result = std::make_unique<OrConditionEditor>();
    result->SetValue(or_condition);
    return result;
  } else if (auto click_state_condition =
                 dynamic_cast<ui::style::ClickStateCondition*>(condition)) {
    auto result = std::make_unique<ClickStateConditionEditor>();
    result->SetValue(click_state_condition);
    return result;
  } else if (auto focus_condition =
                 dynamic_cast<ui::style::FocusCondition*>(condition)) {
    auto result = std::make_unique<FocusConditionEditor>();
    result->SetValue(focus_condition);
    return result;
  } else if (auto checked_condition =
                 dynamic_cast<ui::style::CheckedCondition*>(condition)) {
    auto result = std::make_unique<CheckedConditionEditor>();
    result->SetValue(checked_condition);
    return result;
  } else {
    return nullptr;
  }
}
}  // namespace cru::theme_builder::components::conditions
