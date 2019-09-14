#pragma once
#include "control.hpp"

#include <cstdlib>
#include <forward_list>
#include <optional>

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

class ClickDetector : public Object {
 public:
  explicit ClickDetector(Control* control);

  ClickDetector(const ClickDetector& other) = delete;
  ClickDetector& operator=(const ClickDetector& other) = delete;
  ClickDetector(ClickDetector&& other) = delete;
  ClickDetector& operator=(ClickDetector&& other) = delete;

  ~ClickDetector() override = default;

  Control* GetControl() const { return control_; }

  // Return a union of buttons being pressed. Return 0 if no button is being
  // pressed.
  MouseButton GetPressingButton() const {
    unsigned result = 0;
    if (click_map_.left.has_value()) result |= MouseButton::Left;
    if (click_map_.middle.has_value()) result |= MouseButton::Middle;
    if (click_map_.right.has_value()) result |= MouseButton::Right;
    return static_cast<MouseButton>(result);
  }

  IEvent<ClickEventArgs>* ClickEvent() { return &event_; }

  IEvent<MouseButton>* ClickBeginEvent() { return &begin_event_; }
  IEvent<MouseButton>* ClickEndEvent() { return &end_event_; }

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

  Event<ClickEventArgs> event_;

  Event<MouseButton> begin_event_;
  Event<MouseButton> end_event_;

  std::forward_list<EventRevokerGuard> event_rovoker_guards_;

  struct {
    std::optional<Point> left = std::nullopt;
    std::optional<Point> middle = std::nullopt;
    std::optional<Point> right = std::nullopt;
  } click_map_;
};
}  // namespace cru::ui
