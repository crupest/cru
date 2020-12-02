#include "cru/ui/style/Condition.hpp"
#include <memory>

#include "cru/common/Event.hpp"
#include "cru/ui/controls/Control.hpp"
#include "cru/ui/controls/IClickableControl.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

namespace cru::ui::style {
CompoundCondition::CompoundCondition(
    std::vector<std::unique_ptr<Condition>> conditions)
    : conditions_(std::move(conditions)) {
  for (const auto& p : conditions_) readonly_conditions_.push_back(p.get());
}

std::vector<IBaseEvent*> CompoundCondition::ChangeOn(
    controls::Control* control) const {
  std::vector<IBaseEvent*> result;

  for (auto condition : GetConditions()) {
    for (auto e : condition->ChangeOn(control)) {
      result.push_back(e);
    }
  }

  return result;
}

std::vector<std::unique_ptr<Condition>> CompoundCondition::CloneConditions()
    const {
  std::vector<std::unique_ptr<Condition>> result;
  for (auto condition : GetConditions()) {
    result.push_back(condition->Clone());
  }
  return result;
}

bool AndCondition::Judge(controls::Control* control) const {
  for (auto condition : GetConditions()) {
    if (!condition->Judge(control)) return false;
  }
  return true;
}

bool OrCondition::Judge(controls::Control* control) const {
  for (auto condition : GetConditions()) {
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
}  // namespace cru::ui::style
