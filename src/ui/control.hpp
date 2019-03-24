#pragma once
#include "pre.hpp"

#include "base.hpp"
#include "events/ui_event.hpp"
#include "ui_base.hpp"

namespace cru::ui {
class Window;

namespace render {
class RenderObject;
}

class Control : public Object {
  friend class Window;

 protected:
  Control() = default;

 public:
  Control(const Control& other) = delete;
  Control(Control&& other) = delete;
  Control& operator=(const Control& other) = delete;
  Control& operator=(Control&& other) = delete;
  ~Control() override = default;

 public:
  virtual StringView GetControlType() const = 0;

  //*************** region: tree ***************
 public:
  // Get the window if attached, otherwise, return nullptr.
  Window* GetWindow() const { return window_; }

  Control* GetParent() const { return parent_; }

  virtual const std::vector<Control*>& GetChildren() const = 0;

  // Traverse the tree rooted the control including itself.
  void TraverseDescendants(const std::function<void(Control*)>& predicate);

  void _SetParent(Control* parent);
  void _SetDescendantWindow(Window* window);

 private:
  static void _TraverseDescendants(
      Control* control, const std::function<void(Control*)>& predicate);

 public:
  virtual render::RenderObject* GetRenderObject() const = 0;

  //*************** region: focus ***************
 public:
  bool RequestFocus();

  bool HasFocus();

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control.
  events::RoutedEvent<events::MouseEventArgs>* MouseEnterEvent() {
    return &mouse_enter_event_;
  }
  // Raised when mouse is leave the control.
  events::RoutedEvent<events::MouseEventArgs>* MouseLeaveEvent() {
    return &mouse_leave_event_;
  }
  // Raised when mouse is move in the control.
  events::RoutedEvent<events::MouseEventArgs>* MouseMoveEvent() {
    return &mouse_move_event_;
  }
  // Raised when a mouse button is pressed in the control.
  events::RoutedEvent<events::MouseButtonEventArgs>* MouseDownEvent() {
    return &mouse_down_event_;
  }
  // Raised when a mouse button is released in the control.
  events::RoutedEvent<events::MouseButtonEventArgs>* MouseUpEvent() {
    return &mouse_up_event_;
  }
  // Raised when a mouse button is pressed in the control and released in the
  // control with mouse not leaving it between two operations.
  events::RoutedEvent<events::MouseButtonEventArgs>* MouseClickEvent() {
    return &mouse_click_event_;
  }
  events::RoutedEvent<events::MouseWheelEventArgs>* MouseWheelEvent() {
    return &mouse_wheel_event_;
  }
  events::RoutedEvent<events::KeyEventArgs>* KeyDownEvent() {
    return &key_down_event_;
  }
  events::RoutedEvent<events::KeyEventArgs>* KeyUpEvent() {
    return &key_up_event_;
  }
  events::RoutedEvent<events::CharEventArgs>* CharEvent() {
    return &char_event_;
  }
  events::RoutedEvent<events::FocusChangeEventArgs>* GainFocusEvent() {
    return &gain_focus_event_;
  }
  events::RoutedEvent<events::FocusChangeEventArgs>* LoseFocusEvent() {
    return &lose_focus_event_;
  }

 private:
  events::RoutedEvent<events::MouseEventArgs> mouse_enter_event_;
  events::RoutedEvent<events::MouseEventArgs> mouse_leave_event_;
  events::RoutedEvent<events::MouseEventArgs> mouse_move_event_;
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_down_event_;
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_up_event_;
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_click_event_;
  events::RoutedEvent<events::MouseWheelEventArgs> mouse_wheel_event_;

  events::RoutedEvent<events::KeyEventArgs> key_down_event_;
  events::RoutedEvent<events::KeyEventArgs> key_up_event_;
  events::RoutedEvent<events::CharEventArgs> char_event_;

  events::RoutedEvent<events::FocusChangeEventArgs> gain_focus_event_;
  events::RoutedEvent<events::FocusChangeEventArgs> lose_focus_event_;

  //*************** region: tree ***************
 protected:
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);
  virtual void OnAttachToWindow(Window* window);
  virtual void OnDetachToWindow(Window* window);

  //*************** region: additional mouse event ***************
 protected:
  virtual void OnMouseClickBegin(MouseButton button);
  virtual void OnMouseClickEnd(MouseButton button);

 private:
  Window* window_ = nullptr;
  Control* parent_ = nullptr;
};
}  // namespace cru::ui
