#pragma once
#include "Control.hpp"

namespace cru::ui {
class ClickEventArgs : Object {
 public:
  ClickEventArgs(Control* sender, const Point& down_point,
                 const Point& up_point, MouseButton button)
      : sender_(sender),
        down_point_(down_point),
        up_point_(up_point),
        button_(button) {}

  CRU_DEFAULT_COPY(ClickEventArgs)
  CRU_DEFAULT_MOVE(ClickEventArgs)

  ~ClickEventArgs() override = default;

  Control* GetSender() const { return sender_; }
  Point GetDownPoint() const { return down_point_; }
  Point GetUpPoint() const { return up_point_; }
  MouseButton GetButton() const { return button_; }

 private:
  Control* sender_;
  Point down_point_;
  Point up_point_;
  MouseButton button_;
};

enum class ClickState {
  None,          // Mouse is outside the control.
  Hover,         // Mouse hovers on the control but not pressed
  Press,         // Mouse is pressed and if released click is done.
  PressInactive  // Mouse is pressed but if released click is canceled.
};

class ClickDetector : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::ClickDetector")

 public:
  explicit ClickDetector(Control* control);

  CRU_DELETE_COPY(ClickDetector)
  CRU_DELETE_MOVE(ClickDetector)

  ~ClickDetector() override = default;

  Control* GetControl() const { return control_; }

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

  IEvent<ClickEventArgs>* ClickEvent() { return &event_; }

  IEvent<ClickState>* StateChangeEvent() { return &state_change_event_; }

 private:
  void SetState(ClickState state);

 private:
  Control* control_;

  ClickState state_;

  bool enable_ = true;
  MouseButton trigger_button_ = mouse_buttons::left | mouse_buttons::right;

  Event<ClickEventArgs> event_;
  Event<ClickState> state_change_event_;

  std::vector<EventRevokerGuard> event_rovoker_guards_;

  Point down_point_;
  MouseButton button_;
};
}  // namespace cru::ui
