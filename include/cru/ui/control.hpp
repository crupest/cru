#pragma once
#include "base.hpp"
#include "cru/common/base.hpp"

#include "cru/platform/native/basic_types.hpp"
#include "event/ui_event.hpp"

#include <string_view>
#include <vector>

namespace cru::ui {
class Window;
namespace render {
class RenderObject;
}  // namespace render

class Control : public Object {
  friend class Window;

 protected:
  Control();

 public:
  Control(const Control& other) = delete;
  Control(Control&& other) = delete;
  Control& operator=(const Control& other) = delete;
  Control& operator=(Control&& other) = delete;
  ~Control() override = default;

 public:
  virtual std::wstring_view GetControlType() const = 0;

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

  //*************** region: mouse ***************
 public:
  bool IsMouseOver() const { return is_mouse_over_; }

  bool CaptureMouse();

  bool ReleaseMouse();

  bool IsMouseCaptured();

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control.
  event::RoutedEvent<event::MouseEventArgs>* MouseEnterEvent() {
    return &mouse_enter_event_;
  }
  // Raised when mouse is leave the control.
  event::RoutedEvent<event::MouseEventArgs>* MouseLeaveEvent() {
    return &mouse_leave_event_;
  }
  // Raised when mouse is move in the control.
  event::RoutedEvent<event::MouseEventArgs>* MouseMoveEvent() {
    return &mouse_move_event_;
  }
  // Raised when a mouse button is pressed in the control.
  event::RoutedEvent<event::MouseButtonEventArgs>* MouseDownEvent() {
    return &mouse_down_event_;
  }
  // Raised when a mouse button is released in the control.
  event::RoutedEvent<event::MouseButtonEventArgs>* MouseUpEvent() {
    return &mouse_up_event_;
  }
  event::RoutedEvent<event::MouseWheelEventArgs>* MouseWheelEvent() {
    return &mouse_wheel_event_;
  }
  event::RoutedEvent<event::KeyEventArgs>* KeyDownEvent() {
    return &key_down_event_;
  }
  event::RoutedEvent<event::KeyEventArgs>* KeyUpEvent() {
    return &key_up_event_;
  }
  //  event::RoutedEvent<event::CharEventArgs>* CharEvent() {
  //    return &char_event_;
  //  }
  event::RoutedEvent<event::FocusChangeEventArgs>* GainFocusEvent() {
    return &gain_focus_event_;
  }
  event::RoutedEvent<event::FocusChangeEventArgs>* LoseFocusEvent() {
    return &lose_focus_event_;
  }

 private:
  event::RoutedEvent<event::MouseEventArgs> mouse_enter_event_;
  event::RoutedEvent<event::MouseEventArgs> mouse_leave_event_;
  event::RoutedEvent<event::MouseEventArgs> mouse_move_event_;
  event::RoutedEvent<event::MouseButtonEventArgs> mouse_down_event_;
  event::RoutedEvent<event::MouseButtonEventArgs> mouse_up_event_;
  event::RoutedEvent<event::MouseWheelEventArgs> mouse_wheel_event_;

  event::RoutedEvent<event::KeyEventArgs> key_down_event_;
  event::RoutedEvent<event::KeyEventArgs> key_up_event_;
  //  event::RoutedEvent<event::CharEventArgs> char_event_;

  event::RoutedEvent<event::FocusChangeEventArgs> gain_focus_event_;
  event::RoutedEvent<event::FocusChangeEventArgs> lose_focus_event_;

  //*************** region: tree ***************
 protected:
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);
  virtual void OnAttachToWindow(Window* window);
  virtual void OnDetachToWindow(Window* window);

 private:
  Window* window_ = nullptr;
  Control* parent_ = nullptr;

 private:
  bool is_mouse_over_ = false;

  struct {
    bool left;
    bool middle;
    bool right;
  } click_map_;
};
}  // namespace cru::ui
