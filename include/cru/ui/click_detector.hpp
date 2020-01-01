#pragma once
#include "control.hpp"

#include <optional>
#include <vector>

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
 public:
  explicit ClickDetector(Control* control);

  CRU_DELETE_COPY(ClickDetector)
  CRU_DELETE_MOVE(ClickDetector)

  ~ClickDetector() override = default;

  Control* GetControl() const { return control_; }

  MouseButton GetTriggerButton() const { return trigger_button_; }
  void SetTriggerButton(MouseButton trigger_button);

  IEvent<ClickEventArgs>* ClickEvent() { return &event_; }

  IEvent<ClickState>* StateChangeEvent() { return &state_change_event_; }

 private:
  std::optional<Point>& FromButton(MouseButton button) {
    switch (button) {
      case MouseButton::Left:
        return click_map_.left;
      case MouseButton::Middle:
        return click_map_.middle;
      case MouseButton::Right:
        return click_map_.right;
      default:
        UnreachableCode();
    }
  }

 private:
  Control* control_;

  MouseButton trigger_button_ = MouseButton::Left | MouseButton::Right;

  Event<ClickEventArgs> event_;
  Event<ClickState> state_change_event_;

  std::vector<EventRevokerGuard> event_rovoker_guards_;

  struct {
    std::optional<Point> left = std::nullopt;
    std::optional<Point> middle = std::nullopt;
    std::optional<Point> right = std::nullopt;
  } click_map_;
};
}  // namespace cru::ui
