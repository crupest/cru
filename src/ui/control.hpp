#pragma once
#include "pre.hpp"

#include "system_headers.hpp"

#include "base.hpp"
#include "cursor.hpp"
#include "events/ui_event.hpp"
#include "input_util.hpp"
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

  //*************** region: cursor ***************
  // If cursor is set to null, then it uses parent's cursor.
  // Window's cursor can't be null.
 public:
  Cursor::Ptr GetCursor() const { return cursor_; }

  void SetCursor(const Cursor::Ptr& cursor);

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control.
  events::RoutedEvent<events::MouseEventArgs> mouse_enter_event;
  // Raised when mouse is leave the control.
  events::RoutedEvent<events::MouseEventArgs> mouse_leave_event;
  // Raised when mouse is move in the control.
  events::RoutedEvent<events::MouseEventArgs> mouse_move_event;
  // Raised when a mouse button is pressed in the control.
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_down_event;
  // Raised when a mouse button is released in the control.
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_up_event;
  // Raised when a mouse button is pressed in the control and released in the
  // control with mouse not leaving it between two operations.
  events::RoutedEvent<events::MouseButtonEventArgs> mouse_click_event;

  events::RoutedEvent<events::MouseWheelEventArgs> mouse_wheel_event;

  events::RoutedEvent<events::KeyEventArgs> key_down_event;
  events::RoutedEvent<events::KeyEventArgs> key_up_event;
  events::RoutedEvent<events::CharEventArgs> char_event;

  events::RoutedEvent<events::FocusChangeEventArgs> get_focus_event;
  events::RoutedEvent<events::FocusChangeEventArgs> lose_focus_event;

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

  Cursor::Ptr cursor_{};
};
}  // namespace cru::ui
