#include "ConditionEditor.h"
#include "../Common.h"
#include "CheckedConditionEditor.h"
#include "ClickStateConditionEditor.h"
#include "CompoundConditionEditor.h"
#include "FocusConditionEditor.h"
#include "NoConditionEditor.h"
#include "cru/base/Exception.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::conditions {
ConditionEditor::ConditionEditor() {}

ConditionEditor::~ConditionEditor() {}

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
  } else if (auto no_condition =
                 dynamic_cast<ui::style::NoCondition*>(condition)) {
    auto result = std::make_unique<NoConditionEditor>();
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
    throw Exception(u"Unknown condition type");
  }
}
}  // namespace cru::theme_builder::components::conditions
