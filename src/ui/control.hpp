#pragma once
#include "pre.hpp"

#include <any>
#include <unordered_map>
#include <utility>
#include "system_headers.hpp"

#include "base.hpp"
#include "cursor.hpp"
#include "events/ui_event.hpp"
#include "input_util.hpp"
#include "ui_base.hpp"

namespace cru::ui {
class Window;

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
  static void TraverseDescendantsInternal(
      Control* control, const std::function<void(Control*)>& predicate);

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

class NoChildControl : public Control {
 private:
  static const std::vector<Control*> empty_control_vector;

 protected:
  NoChildControl() = default;

 public:
  NoChildControl(const NoChildControl& other) = delete;
  NoChildControl(NoChildControl&& other) = delete;
  NoChildControl& operator=(const NoChildControl& other) = delete;
  NoChildControl& operator=(NoChildControl&& other) = delete;
  ~NoChildControl() override = default;

 protected:
  const std::vector<Control*>& GetChildren() const override final {
    return empty_control_vector;
  }
};

class ContentControl : public Control {
 protected:
  ContentControl();

 public:
  ContentControl(const ContentControl& other) = delete;
  ContentControl(ContentControl&& other) = delete;
  ContentControl& operator=(const ContentControl& other) = delete;
  ContentControl& operator=(ContentControl&& other) = delete;
  ~ContentControl() override;

  const std::vector<Control*>& GetChildren() const override final {
    return child_vector_;
  }
  Control* GetChild() const { return child_; }
  void SetChild(Control* child);

 protected:
  virtual void OnChildChanged(Control* old_child, Control* new_child);

 private:
  std::vector<Control*> child_vector_;
  Control*& child_;
};

class MultiChildControl : public Control {
 protected:
  MultiChildControl() = default;

 public:
  MultiChildControl(const MultiChildControl& other) = delete;
  MultiChildControl(MultiChildControl&& other) = delete;
  MultiChildControl& operator=(const MultiChildControl& other) = delete;
  MultiChildControl& operator=(MultiChildControl&& other) = delete;
  ~MultiChildControl() override;

  const std::vector<Control*>& GetChildren() const override final {
    return children_;
  }

  void AddChild(Control* control, int position);

  void RemoveChild(int position);

 protected:
  virtual void OnAddChild(Control* child);
  virtual void OnRemoveChild(Control* child);

 private:
  std::vector<Control*> children_;
};

//*************** region: event dispatcher helper ***************

// Dispatch the event.
//
// This will raise routed event of the control and its parent and parent's
// parent ... (until "last_receiver" if it's not nullptr) with appropriate args.
//
// First tunnel from top to bottom possibly stopped by "handled" flag in
// EventArgs. Second bubble from bottom to top possibly stopped by "handled"
// flag in EventArgs. Last direct to each control.
//
// Args is of type "EventArgs". The first init argument is "sender", which is
// automatically bound to each receiving control. The second init argument is
// "original_sender", which is unchanged. And "args" will be perfectly forwarded
// as the rest arguments.
template <typename EventArgs, typename... Args>
void DispatchEvent(Control* const original_sender,
                   events::RoutedEvent<EventArgs> Control::*event_ptr,
                   Control* const last_receiver, Args&&... args) {
  std::list<Control*> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent);
    parent = parent->GetInternalParent();
  }

  auto handled = false;

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
    EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
    (*i->*event_ptr).tunnel.Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
      break;
    }
  }

  // bubble
  if (!handled) {
    for (auto i : receive_list) {
      EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
      (i->*event_ptr).bubble.Raise(event_args);
      if (event_args.IsHandled()) break;
    }
  }

  // direct
  for (auto i : receive_list) {
    EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
    (i->*event_ptr).direct.Raise(event_args);
  }
}

//*************** region: tree helper ***************

// Find the lowest common ancestor.
// Return nullptr if "left" and "right" are not in the same tree.
Control* FindLowestCommonAncestor(Control* left, Control* right);

}  // namespace cru::ui
