#include "ClickStateConditionEditor.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
using ui::helper::ClickState;
namespace {
const std::vector<std::string> kClickStates{
    "None",
    "Hover",
    "Press",
    "PressInactive",
};

Index ConvertClickStateToIndex(ClickState click_state) {
  switch (click_state) {
    case ClickState::None:
      return 0;
    case ClickState::Hover:
      return 1;
    case ClickState::Press:
      return 2;
    case ClickState::PressInactive:
      return 3;
  }
  return -1;
}

ClickState ConvertIndexToClickState(Index index) {
  switch (index) {
    case 0:
      return ClickState::None;
    case 1:
      return ClickState::Hover;
    case 2:
      return ClickState::Press;
    case 3:
      return ClickState::PressInactive;
  }
  return ClickState::None;
}
}  // namespace

ClickStateConditionEditor::ClickStateConditionEditor() {
  SetLabel("Click State Condition");
  GetContainer()->AddChild(click_state_select_.GetRootControl());

  click_state_select_.SetLabel("Click State");
  click_state_select_.SetItems(kClickStates);
  click_state_select_.SetSelectedIndex(0, false);

  ConnectChangeEvent(click_state_select_);
}

ClickStateConditionEditor::~ClickStateConditionEditor() {}

ClonablePtr<ui::style::ClickStateCondition>
ClickStateConditionEditor::GetValue() const {
  return ui::style::ClickStateCondition::Create(
      ConvertIndexToClickState(click_state_select_.GetSelectedIndex()));
}

void ClickStateConditionEditor::SetValue(ui::style::ClickStateCondition* value,
                                         bool trigger_change) {
  click_state_select_.SetSelectedIndex(
      ConvertClickStateToIndex(value->GetClickState()), trigger_change);
}
}  // namespace cru::theme_builder::components::conditions
