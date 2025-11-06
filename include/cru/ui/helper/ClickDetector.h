#pragma once
#include "../Base.h"

#include "cru/base/Event.h"

namespace cru::ui::helper {
class ClickDetector;

class CRU_UI_API ClickEventArgs : Object {
 public:
  ClickEventArgs(controls::Control* sender, const Point& down_point,
                 const Point& up_point, MouseButton button)
      : sender_(sender),
        down_point_(down_point),
        up_point_(up_point),
        button_(button) {}

  controls::Control* GetSender() const { return sender_; }
  Point GetDownPoint() const { return down_point_; }
  Point GetDownPointOfScreen() const;
  Point GetUpPoint() const { return up_point_; }
  MouseButton GetButton() const { return button_; }

 private:
  controls::Control* sender_;
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
  CRU_DEFINE_CLASS_LOG_TAG("ClickDetector")

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

  IEvent<const ClickEventArgs&>* ClickEvent() { return &event_; }

  IEvent<ClickState>* StateChangeEvent() { return &state_change_event_; }

 private:
  void SetState(ClickState state);

 private:
  controls::Control* control_;

  ClickState state_ = ClickState::None;

  bool enable_ = true;
  MouseButton trigger_button_ = MouseButtons::Left | MouseButtons::Right;

  Event<const ClickEventArgs&> event_;
  Event<ClickState> state_change_event_;

  std::vector<EventHandlerRevokerGuard> event_rovoker_guards_;

  Point down_point_;
  MouseButton button_;
};
}  // namespace cru::ui::helper
