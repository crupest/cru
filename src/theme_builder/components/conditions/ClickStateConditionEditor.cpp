#include "ClickStateConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
using ui::helper::ClickState;
namespace {
const std::vector<String> kClickStates{
    u"None",
    u"Hover",
    u"Press",
    u"PressInactive",
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
  GetContainer()->AddChild(click_state_select_.GetRootControl());

  click_state_select_.SetItems(kClickStates);
  click_state_select_.SetSelectedIndex(0, false);

  click_state_select_.ChangeEvent()->AddSpyOnlyHandler(
      [this] { change_event_.Raise(nullptr); });
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
