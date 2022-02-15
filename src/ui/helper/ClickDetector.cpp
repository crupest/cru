#include "cru/ui/helper/ClickDetector.h"

#include "cru/common/Logger.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/host/WindowHost.h"

#include <optional>

namespace cru::ui::helper {
Point ClickEventArgs::GetDownPointOfScreen() const {
  auto window_host = sender_->GetWindowHost();
  if (window_host != nullptr) {
    auto window = window_host->GetNativeWindow();
    return down_point_ + window->GetClientRect().GetLeftTop();
  } else {
    return down_point_;
  }
}

ClickDetector::ClickDetector(controls::Control* control) {
  Expects(control);
  control_ = control;

  event_rovoker_guards_.push_back(
      EventRevokerGuard(control->MouseEnterEvent()->Direct()->AddHandler(
          [this](events::MouseEventArgs&) {
            if (this->enable_) {
              if (this->state_ == ClickState::PressInactive) {
                if ((this->button_ & this->trigger_button_)) {
                  this->SetState(ClickState::Press);
                }
              } else {
                this->SetState(ClickState::Hover);
              }
            }
          })));

  event_rovoker_guards_.push_back(
      EventRevokerGuard(control->MouseLeaveEvent()->Direct()->AddHandler(
          [this](events::MouseEventArgs&) {
            if (this->enable_) {
              if (this->state_ == ClickState::Press) {
                if ((this->button_ & this->trigger_button_)) {
                  this->SetState(ClickState::PressInactive);
                }
              } else {
                this->SetState(ClickState::None);
              }
            }
          })));

  event_rovoker_guards_.push_back(
      EventRevokerGuard(control->MouseDownEvent()->Direct()->AddHandler(
          [this](events::MouseButtonEventArgs& args) {
            const auto button = args.GetButton();
            if (this->enable_ && (button & this->trigger_button_) &&
                this->state_ == ClickState::Hover) {
              if (!this->control_->CaptureMouse()) {
                if constexpr (debug_flags::click_detector) {
                  log::TagDebug(log_tag,
                                u"Failed to capture mouse when begin click.");
                }
                return;
              }
              this->down_point_ = args.GetPoint();
              this->button_ = button;
              this->SetState(ClickState::Press);
            }
          })));

  event_rovoker_guards_.push_back(
      EventRevokerGuard(control->MouseUpEvent()->Direct()->AddHandler(
          [this](events::MouseButtonEventArgs& args) {
            const auto button = args.GetButton();
            if (this->enable_ && (button & this->trigger_button_) &&
                button == button_) {
              if (this->state_ == ClickState::Press) {
                this->SetState(ClickState::Hover);
                this->event_.Raise(ClickEventArgs{this->control_,
                                                  this->down_point_,
                                                  args.GetPoint(), button});
                this->control_->ReleaseMouse();
              } else if (this->state_ == ClickState::PressInactive) {
                this->SetState(ClickState::None);
                this->control_->ReleaseMouse();
              }
            }
          })));
}  // namespace cru::ui

void ClickDetector::SetEnabled(bool enable) {
  if (enable == enable_) {
    return;
  }

  enable_ = enable;
  if (enable) {
    SetState(control_->IsMouseOver() ? ClickState::Hover : ClickState::None);
  } else {
    if (state_ == ClickState::Press || state_ == ClickState::PressInactive) {
      SetState(ClickState::None);
      control_->ReleaseMouse();
    } else if (state_ == ClickState::Hover) {
      SetState(ClickState::None);
    }
  }
}

void ClickDetector::SetTriggerButton(MouseButton trigger_button) {
  if (trigger_button == trigger_button_) {
    return;
  }

  trigger_button_ = trigger_button;
  if ((state_ == ClickState::Press || state_ == ClickState::PressInactive) &&
      !(button_ & trigger_button)) {
    SetState(control_->IsMouseOver() ? ClickState::Hover : ClickState::None);
    control_->ReleaseMouse();
  }
}

void ClickDetector::SetState(ClickState state) {
  if constexpr (debug_flags::click_detector) {
    auto to_string = [](ClickState state) -> std::u16string_view {
      switch (state) {
        case ClickState::None:
          return u"None";
        case ClickState::Hover:
          return u"Hover";
        case ClickState::Press:
          return u"Press";
        case ClickState::PressInactive:
          return u"PressInvactive";
        default:
          UnreachableCode();
      }
    };
    log::TagDebug(log_tag, u"Click state changed, new state: {}.",
                  to_string(state));
  }

  state_ = state;
  state_change_event_.Raise(state);
}
}  // namespace cru::ui::helper
