#pragma once
#include "../Base.h"
#include "../events/UiEvents.h"
#include "../render/RenderObject.h"

namespace cru::ui::controls {

/**
 *  \remarks If you want to write a new control. You should override following
 * methods:
 *  - GetControlType()
 *  - ForEachChild(const std::function<void(Control*)>& predicate)
 *  - GetRenderObject()
 */
class CRU_UI_API Control : public Object {
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
  virtual host::WindowHost* GetWindowHost() const;

  Control* GetParent() const { return parent_; }
  void SetParent(Control* parent);

  virtual void ForEachChild(const std::function<void(Control*)>& predicate) = 0;

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
  std::shared_ptr<style::StyleRuleSet> GetStyleRuleSet();

  //*************** region: events ***************
 public:
  // Raised when mouse enter the control. Even when the control itself captures
  // the mouse, this event is raised as regular. But if mouse is captured by
  // another control, the control will not receive any mouse enter event. You
  // can use `IsMouseCaptured` to get more info.
  events::RoutedEvent<events::MouseEventArgs>* MouseEnterEvent() {
    return &mouse_enter_event_;
  }
  // Raised when mouse is leave the control. Even when the control itself
  // captures the mouse, this event is raised as regular. But if mouse is
  // captured by another control, the control will not receive any mouse leave
  // event. You can use `IsMouseCaptured` to get more info.
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
  events::RoutedEvent<events::MouseWheelEventArgs>* MouseWheelEvent() {
    return &mouse_wheel_event_;
  }
  events::RoutedEvent<events::KeyEventArgs>* KeyDownEvent() {
    return &key_down_event_;
  }
  events::RoutedEvent<events::KeyEventArgs>* KeyUpEvent() {
    return &key_up_event_;
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
  events::RoutedEvent<events::MouseWheelEventArgs> mouse_wheel_event_;

  events::RoutedEvent<events::KeyEventArgs> key_down_event_;
  events::RoutedEvent<events::KeyEventArgs> key_up_event_;

  events::RoutedEvent<events::FocusChangeEventArgs> gain_focus_event_;
  events::RoutedEvent<events::FocusChangeEventArgs> lose_focus_event_;

  //*************** region: tree ***************
 protected:
  virtual void OnParentChanged(Control* old_parent, Control* new_parent);

 protected:
  virtual void OnMouseHoverChange(bool newHover) { CRU_UNUSED(newHover) }

 private:
  Control* parent_ = nullptr;

 private:
  bool is_mouse_over_ = false;

  std::shared_ptr<platform::gui::ICursor> cursor_ = nullptr;

  std::shared_ptr<style::StyleRuleSet> style_rule_set_;
  std::unique_ptr<style::StyleRuleSetBind> style_rule_set_bind_;
};
}  // namespace cru::ui::controls
