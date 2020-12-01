#include "cru/ui/style/Trigger.hpp"

#include "cru/ui/controls/Control.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

namespace cru::ui::style {
void Trigger::Raise(bool value) {
  if (value == current_) return;
  current_ = value;
  change_event_.Raise(value);
}

CompoundTrigger::CompoundTrigger(std::vector<Trigger*> triggers)
    : triggers_(std::move(triggers)) {
  for (auto trigger : triggers_) {
    guard_ += trigger->ChangeEvent()->AddHandler(
        [this](bool) { Raise(this->CalculateState(triggers_)); });
  }
}

bool AndTrigger::CalculateState(const std::vector<Trigger*>& triggers) const {
  for (auto trigger : triggers) {
    if (!trigger->GetState()) return false;
  }
  return true;
}

bool OrTrigger::CalculateState(const std::vector<Trigger*>& triggers) const {
  for (auto trigger : triggers) {
    if (trigger->GetState()) return true;
  }
  return false;
}

FocusTrigger::FocusTrigger(controls::Control* control, bool has_focus)
    : has_focus_(has_focus) {
  guard_ += control->GainFocusEvent()->Direct()->AddHandler(
      [this](auto) { Raise(has_focus_); });
  guard_ += control->LoseFocusEvent()->Direct()->AddHandler(
      [this](auto) { Raise(!has_focus_); });
}

ClickStateTrigger::ClickStateTrigger(controls::IClickableControl* control,
                                     helper::ClickState click_state)
    : click_state_(click_state) {
  guard_ += control->ClickStateChangeEvent()->AddHandler(
      [this](helper::ClickState cs) { Raise(cs == click_state_); });
}
}  // namespace cru::ui::style
