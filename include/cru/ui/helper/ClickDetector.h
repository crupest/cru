#pragma once
#include "../Base.h"

#include "cru/base/Event.h"

#include <chrono>

namespace cru::ui::helper {
class ClickDetector;

class CRU_UI_API ClickEventArgs : Object {
 public:
  ClickEventArgs(controls::Control* sender, const Point& down_point,
                 const Point& up_point, MouseButton button,
                 int click_count = 1)
      : sender_(sender),
        down_point_(down_point),
        up_point_(up_point),
        button_(button),
        click_count_(click_count) {}

  controls::Control* GetSender() const { return sender_; }
  Point GetDownPoint() const { return down_point_; }
  Point GetDownPointOfScreen() const;
  Point GetUpPoint() const { return up_point_; }
  MouseButton GetButton() const { return button_; }
  int GetClickCount() const { return click_count_; }

 private:
  controls::Control* sender_;
  Point down_point_;
  Point up_point_;
  MouseButton button_;
  int click_count_;
};

enum class ClickState {
  None,          // Mouse is outside the control.
  Hover,         // Mouse hovers on the control but not pressed
  Press,         // Mouse is pressed and if released click is done.
  PressInactive  // Mouse is pressed but if released click is canceled.
};

class ClickDetector : public Object {
 private:
  constexpr static auto kLogTag = "ClickDetector";

 public:
  explicit ClickDetector(controls::Control* control);
  ~ClickDetector() override = default;

  controls::Control* GetControl() const { return control_; }

  ClickState GetState() const { return state_; }

  // Default is enable.
  bool IsEnabled() const { return enable_; }
  // If disable when user is pressing, the pressing is deactivated.
  void SetEnabled(bool enable);

  // Default is left and right.
  MouseButton GetTriggerButton() const { return trigger_button_; }
  // If unset the trigger button when user is pressing, the pressing is
  // deactivated.
  void SetTriggerButton(MouseButton trigger_button);

  // Default is {4, 4}.
  Point GetMultipleClickPositionOffsetThreshold() const {
    return multiple_click_position_offset_threshold_;
  }
  void SetMultipleClickPositionOffsetThreshold(Point threshold);

  // Default is 500ms.
  std::chrono::milliseconds GetMultipleClickDurationThreshold() const {
    return multiple_click_duration_threshold_;
  }
  void SetMultipleClickDurationThreshold(
      std::chrono::milliseconds duration_threshold);

  IEvent<const ClickEventArgs&>* ClickEvent() { return &event_; }
  IEvent<const ClickEventArgs&>* DoubleClickEvent() {
    return &double_click_event_;
  }

  IEvent<ClickState>* StateChangeEvent() { return &state_change_event_; }

 private:
  void SetState(ClickState state);
  int NextClickCount(const Point& up_point, MouseButton button,
                     std::chrono::steady_clock::time_point time);
  bool IsWithinMultipleClickPositionOffsetThreshold(const Point& point) const;
  void ResetMultipleClick();

 private:
  controls::Control* control_;

  ClickState state_ = ClickState::None;

  bool enable_ = true;
  MouseButton trigger_button_ = MouseButtons::Left | MouseButtons::Right;

  Point multiple_click_position_offset_threshold_ = {4.f, 4.f};
  std::chrono::milliseconds multiple_click_duration_threshold_{500};

  Event<const ClickEventArgs&> event_;
  Event<const ClickEventArgs&> double_click_event_;
  Event<ClickState> state_change_event_;

  std::vector<EventHandlerRevokerGuard> event_revoker_guards_;

  Point down_point_;
  MouseButton button_;

  int multiple_click_count_ = 0;
  Point last_click_point_;
  MouseButton last_click_button_ = MouseButtons::None;
  std::chrono::steady_clock::time_point last_click_time_{};
};
}  // namespace cru::ui::helper
