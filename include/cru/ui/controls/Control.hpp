#pragma once
#include "Base.hpp"

#include "../events/UiEvent.hpp"
#include "../render/Base.hpp"
#include "cru/common/Event.hpp"

#include <string_view>

namespace cru::ui::controls {
class Control : public Object {
  friend host::WindowHost;

 protected:
  Control();

 public:
  Control(const Control& other) = delete;
  Control(Control&& other) = delete;
  Control& operator=(const Control& other) = delete;
  Control& operator=(Control&& other) = delete;
  ~Control() override;

 public:
  virtual String GetControlType() const = 0;

  //*************** region: tree ***************
 public:
  host::WindowHost* GetWindowHost() const;

  Control* GetParent() const { return parent_; }

  const std::vector<Control*>& GetChildren() const { return children_; }

  // Traverse the tree rooted the control including itself.
  void TraverseDescendants(const std::function<void(Control*)>& predicate);

 public:
  virtual render::RenderObject* GetRenderObject() const = 0;

  //*************** region: focus ***************
 public:
  bool HasFocus();

  void SetFocus();

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
  std::shared_ptr<platform::gui::ICursor> GetCursor();

  // will not return nullptr
  std::shared_ptr<platform::gui::ICursor> GetInheritedCursor();

  // null to unset
  void SetCursor(std::shared_ptr<platform::gui::ICursor> cursor);

 public:
  style::StyleRuleSet* GetStyleRuleSet();

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
  void AddChild(Control* control, Index position);
  void RemoveChild(Index position);
  virtual void OnAddChild(Control* child, Index position);
  virtual void OnRemoveChild(Control* child, Index position);
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);
  virtual void OnAttachToHost(host::WindowHost* host);
  virtual void OnDetachFromHost(host::WindowHost* host);

 protected:
  virtual void OnMouseHoverChange(bool newHover) { CRU_UNUSED(newHover) }

 private:
  Control* parent_ = nullptr;
  std::vector<Control*> children_;

  host::WindowHost* window_host_ = nullptr;

 private:
  bool is_mouse_over_ = false;

  std::shared_ptr<platform::gui::ICursor> cursor_ = nullptr;

  std::unique_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;
};
}  // namespace cru::ui::controls
