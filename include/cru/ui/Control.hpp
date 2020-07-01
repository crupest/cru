#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"
#include "render/Base.hpp"
#include "UiEvent.hpp"

#include <string_view>

namespace cru::ui {
class Control : public Object {
  friend UiHost;

 protected:
  Control();

 public:
  Control(const Control& other) = delete;
  Control(Control&& other) = delete;
  Control& operator=(const Control& other) = delete;
  Control& operator=(Control&& other) = delete;
  ~Control() override = default;

 public:
  virtual std::string_view GetControlType() const = 0;

  //*************** region: tree ***************
 public:
  // Get the ui host if attached, otherwise, return nullptr.
  UiHost* GetUiHost() const { return ui_host_; }

  Control* GetParent() const { return parent_; }

  virtual const std::vector<Control*>& GetChildren() const = 0;

  // Traverse the tree rooted the control including itself.
  void TraverseDescendants(const std::function<void(Control*)>& predicate);

  void _SetParent(Control* parent);
  void _SetDescendantUiHost(UiHost* host);

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

  //*************** region: cursor ***************
  // Cursor is inherited from parent recursively if not set.
 public:
  // null for not set
  std::shared_ptr<platform::native::ICursor> GetCursor();

  // will not return nullptr
  std::shared_ptr<platform::native::ICursor> GetInheritedCursor();

  // null to unset
  void SetCursor(std::shared_ptr<platform::native::ICursor> cursor);

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control. Even when the control itself captures
  // the mouse, this event is raised as regular. But if mouse is captured by
  // another control, the control will not receive any mouse enter event. You
  // can use `IsMouseCaptured` to get more info.
  event::RoutedEvent<event::MouseEventArgs>* MouseEnterEvent() {
    return &mouse_enter_event_;
  }
  // Raised when mouse is leave the control. Even when the control itself
  // captures the mouse, this event is raised as regular. But if mouse is
  // captured by another control, the control will not receive any mouse leave
  // event. You can use `IsMouseCaptured` to get more info.
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

  event::RoutedEvent<event::FocusChangeEventArgs> gain_focus_event_;
  event::RoutedEvent<event::FocusChangeEventArgs> lose_focus_event_;

  //*************** region: tree ***************
 protected:
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);
  virtual void OnAttachToHost(UiHost* host);
  virtual void OnDetachFromHost(UiHost* host);

  virtual void OnMouseHoverChange(bool newHover) { CRU_UNUSED(newHover) }

 private:
  UiHost* ui_host_ = nullptr;
  Control* parent_ = nullptr;

 private:
  bool is_mouse_over_ = false;

  std::shared_ptr<platform::native::ICursor> cursor_ = nullptr;
};
}  // namespace cru::ui