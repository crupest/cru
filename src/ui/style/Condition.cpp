#include "cru/ui/style/Condition.h"
#include <memory>

#include "cru/base/ClonePtr.h"
#include "cru/base/Event.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/ICheckableControl.h"
#include "cru/ui/controls/IClickableControl.h"
#include "cru/ui/helper/ClickDetector.h"

namespace cru::ui::style {
CompoundCondition::CompoundCondition(
    std::vector<ClonePtr<Condition>> conditions)
    : conditions_(std::move(conditions)) {}

std::vector<IBaseEvent*> CompoundCondition::ChangeOn(
    controls::Control* control) const {
  std::vector<IBaseEvent*> result;

  for (auto condition : conditions_) {
    for (auto e : condition->ChangeOn(control)) {
      result.push_back(e);
    }
  }

  return result;
}

bool AndCondition::Judge(controls::Control* control) const {
  for (auto condition : conditions_) {
    if (!condition->Judge(control)) return false;
  }
  return true;
}

bool OrCondition::Judge(controls::Control* control) const {
  for (auto condition : conditions_) {
    if (condition->Judge(control)) return true;
  }
  return false;
}

FocusCondition::FocusCondition(bool has_focus) : has_focus_(has_focus) {}

std::vector<IBaseEvent*> FocusCondition::ChangeOn(
    controls::Control* control) const {
  return {control->GainFocusEvent()->Direct(),
          control->LoseFocusEvent()->Direct()};
}

bool FocusCondition::Judge(controls::Control* control) const {
  return control->HasFocus() == has_focus_;
}

std::vector<IBaseEvent*> HoverCondition::ChangeOn(
    controls::Control* control) const {
  return {control->MouseEnterEvent()->Direct(),
          control->MouseLeaveEvent()->Direct()};
}

bool HoverCondition::Judge(controls::Control* control) const {
  return control->IsMouseOver() == hover_;
}

ClickStateCondition::ClickStateCondition(helper::ClickState click_state)
    : click_state_(click_state) {}

std::vector<IBaseEvent*> ClickStateCondition::ChangeOn(
    controls::Control* control) const {
  auto clickable_control = dynamic_cast<controls::IClickableControl*>(control);
  if (clickable_control) {
    return {clickable_control->ClickStateChangeEvent()};
  } else {
    return {};
  }
}

bool ClickStateCondition::Judge(controls::Control* control) const {
  auto clickable_control = dynamic_cast<controls::IClickableControl*>(control);
  if (clickable_control) {
    return clickable_control->GetClickState() == click_state_;
  }
  return false;
}

CheckedCondition::CheckedCondition(bool checked) : checked_(checked) {}

std::vector<IBaseEvent*> CheckedCondition::ChangeOn(
    controls::Control* control) const {
  auto checkable_control = dynamic_cast<controls::ICheckableControl*>(control);
  if (checkable_control) {
    return {checkable_control->CheckedChangeEvent()};
  } else {
    return {};
  }
}

bool CheckedCondition::Judge(controls::Control* control) const {
  auto checkable_control = dynamic_cast<controls::ICheckableControl*>(control);
  if (checkable_control) {
    return checkable_control->IsChecked() == checked_;
  }
  return false;
}
}  // namespace cru::ui::style
