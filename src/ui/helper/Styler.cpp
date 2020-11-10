#include "cru/ui/helper/Styler.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "gsl/pointers"

namespace cru::ui::helper {
Styler::Styler(gsl::not_null<controls::Control*> control,
               ClickDetector* click_detector)
    : control_(control),
      managed_click_detector_(click_detector ? nullptr
                                             : new ClickDetector(control)),
      click_detector_(click_detector ? click_detector
                                     : managed_click_detector_.get()) {
  event_guard_ += control_->GainFocusEvent()->Direct()->AddHandler(
      [this](auto) { this->RaiseStateChangeEvent(); });
  event_guard_ += control_->LoseFocusEvent()->Direct()->AddHandler(
      [this](auto) { this->RaiseStateChangeEvent(); });
  event_guard_ += click_detector_->StateChangeEvent()->AddHandler(
      [this](auto) { this->RaiseStateChangeEvent(); });
}

Styler::~Styler() = default;

void Styler::RaiseStateChangeEvent() {
  this->state_change_event_.Raise(ControlStyleState{
      this->click_detector_->GetState(), this->control_->HasFocus()});
}
}  // namespace cru::ui::helper
