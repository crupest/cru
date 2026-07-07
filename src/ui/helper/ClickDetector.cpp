#include "cru/ui/helper/ClickDetector.h"

#include "cru/base/log/Logger.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/ControlHost.h"

#include <cmath>

namespace cru::ui::helper {
Point ClickEventArgs::GetDownPointOfScreen() const {
  auto host = sender_->GetControlHost();
  if (host != nullptr) {
    return down_point_ + host->GetNativeWindow()->GetClientRect().GetLeftTop();
  } else {
    return down_point_;
  }
}

ClickDetector::ClickDetector(controls::Control* control) {
  Expects(control);
  control_ = control;

  event_revoker_guards_.push_back(
      EventHandlerRevokerGuard(control->MouseEnterEvent()->Direct()->AddHandler(
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

  event_revoker_guards_.push_back(
      EventHandlerRevokerGuard(control->MouseLeaveEvent()->Direct()->AddHandler(
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

  event_revoker_guards_.push_back(
      EventHandlerRevokerGuard(control->MouseDownEvent()->Direct()->AddHandler(
          [this](events::MouseButtonEventArgs& args) {
            const auto button = args.GetButton();
            if (this->enable_ && (button & this->trigger_button_) &&
                this->state_ == ClickState::Hover) {
              if (!this->control_->CaptureMouse()) {
                CruLogWarn(kLogTag,
                           "Failed to capture mouse when begin click.");
                return;
              }
              this->down_point_ = args.GetPoint();
              this->button_ = button;
              this->SetState(ClickState::Press);
            }
          })));

  event_revoker_guards_.push_back(
      EventHandlerRevokerGuard(control->MouseUpEvent()->Direct()->AddHandler(
          [this](events::MouseButtonEventArgs& args) {
            const auto button = args.GetButton();
            if (this->enable_ && (button & this->trigger_button_) &&
                button == button_) {
              if (this->state_ == ClickState::Press) {
                const auto up_point = args.GetPoint();
                const auto click_count = this->NextClickCount(
                    up_point, button, std::chrono::steady_clock::now());
                this->SetState(ClickState::Hover);
                ClickEventArgs event_args{this->control_, this->down_point_,
                                          up_point, button, click_count};
                this->event_.Raise(event_args);
                if (click_count == 2) {
                  this->double_click_event_.Raise(event_args);
                }
                this->control_->ReleaseMouse();
              } else if (this->state_ == ClickState::PressInactive) {
                this->ResetMultipleClick();
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
    ResetMultipleClick();
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
  ResetMultipleClick();
  if ((state_ == ClickState::Press || state_ == ClickState::PressInactive) &&
      !(button_ & trigger_button)) {
    SetState(control_->IsMouseOver() ? ClickState::Hover : ClickState::None);
    control_->ReleaseMouse();
  }
}

void ClickDetector::SetMultipleClickPositionOffsetThreshold(Point threshold) {
  Expects(threshold.x >= 0.f && threshold.y >= 0.f);

  if (threshold == multiple_click_position_offset_threshold_) {
    return;
  }

  multiple_click_position_offset_threshold_ = threshold;
  ResetMultipleClick();
}

void ClickDetector::SetMultipleClickDurationThreshold(
    std::chrono::milliseconds duration_threshold) {
  Expects(duration_threshold >= std::chrono::milliseconds::zero());

  if (duration_threshold == multiple_click_duration_threshold_) {
    return;
  }

  multiple_click_duration_threshold_ = duration_threshold;
  ResetMultipleClick();
}

int ClickDetector::NextClickCount(const Point& up_point, MouseButton button,
                                  std::chrono::steady_clock::time_point time) {
  if (multiple_click_count_ > 0 && button == last_click_button_ &&
      time - last_click_time_ <= multiple_click_duration_threshold_ &&
      IsWithinMultipleClickPositionOffsetThreshold(up_point)) {
    ++multiple_click_count_;
  } else {
    multiple_click_count_ = 1;
  }

  last_click_point_ = up_point;
  last_click_button_ = button;
  last_click_time_ = time;
  return multiple_click_count_;
}

bool ClickDetector::IsWithinMultipleClickPositionOffsetThreshold(
    const Point& point) const {
  return std::abs(point.x - last_click_point_.x) <=
             multiple_click_position_offset_threshold_.x &&
         std::abs(point.y - last_click_point_.y) <=
             multiple_click_position_offset_threshold_.y;
}

void ClickDetector::ResetMultipleClick() {
  multiple_click_count_ = 0;
  last_click_point_ = {};
  last_click_button_ = MouseButtons::None;
  last_click_time_ = {};
}

void ClickDetector::SetState(ClickState state) {
  state_ = state;
  state_change_event_.Raise(state);
}
}  // namespace cru::ui::helper
