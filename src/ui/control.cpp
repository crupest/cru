#include "cru/ui/control.hpp"

#include "cru/platform/native/basic_types.hpp"
#include "cru/ui/base.hpp"
#include "cru/ui/event/ui_event.hpp"
#include "cru/ui/window.hpp"
#include "routed_event_dispatch.hpp"

#include <cassert>

namespace cru::ui {
void Control::_SetParent(Control* parent) {
  const auto old_parent = GetParent();
  parent_ = parent;
  const auto new_parent = GetParent();
  if (old_parent != new_parent) OnParentChanged(old_parent, new_parent);

  MouseDownEvent()->Direct()->AddHandler(
      [this](event::MouseButtonEventArgs& args) {
        switch (args.GetMouseButton()) {
          case MouseButton::Left:
            click_map_.left = true;
            OnMouseClickBegin(MouseButton::Left);
            break;
          case MouseButton::Middle:
            click_map_.middle = true;
            OnMouseClickBegin(MouseButton::Middle);
            break;
          case MouseButton::Right:
            click_map_.right = true;
            OnMouseClickBegin(MouseButton::Right);
            break;
        }
      });

  MouseEnterEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = true;
  });

  MouseLeaveEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = false;
    if (click_map_.left) {
      OnMouseClickCancel(MouseButton::Left);
    }
    if (click_map_.middle) {
      OnMouseClickCancel(MouseButton::Middle);
    }
    if (click_map_.right) {
      OnMouseClickCancel(MouseButton::Right);
    }
    click_map_.left = click_map_.middle = click_map_.right = false;
  });

  MouseUpEvent()->Direct()->AddHandler([this](event::MouseButtonEventArgs& args) {
    switch (args.GetMouseButton()) {
      case MouseButton::Left:
        if (click_map_.left) {
          click_map_.left = false;
          OnMouseClickEnd(MouseButton::Left);
          DispatchEvent(this, &Control::MouseClickEvent, nullptr, args.GetPoint(), args.GetMouseButton());
        }
        break;
      case MouseButton::Middle:
        if (click_map_.middle) {
          click_map_.middle = false;
          OnMouseClickEnd(MouseButton::Middle);
          DispatchEvent(this, &Control::MouseClickEvent, nullptr, args.GetPoint(), args.GetMouseButton());
        }
        break;
      case MouseButton::Right:
        if (click_map_.right) {
          click_map_.right = false;
          OnMouseClickEnd(MouseButton::Right);
          DispatchEvent(this, &Control::MouseClickEvent, nullptr, args.GetPoint(), args.GetMouseButton());
        }
        break;
    }
  });
}  // namespace cru::ui

void Control::_SetDescendantWindow(Window* window) {
  if (window == nullptr && window_ == nullptr) return;

  // You can only attach or detach window.
  assert((window != nullptr && window_ == nullptr) ||
         (window == nullptr && window_ != nullptr));

  if (window == nullptr) {
    const auto old = window_;
    TraverseDescendants([old](Control* control) {
      control->window_ = nullptr;
      control->OnDetachToWindow(old);
    });
  } else
    TraverseDescendants([window](Control* control) {
      control->window_ = window;
      control->OnAttachToWindow(window);
    });
}

void Control::TraverseDescendants(
    const std::function<void(Control*)>& predicate) {
  _TraverseDescendants(this, predicate);
}

void Control::_TraverseDescendants(
    Control* control, const std::function<void(Control*)>& predicate) {
  predicate(control);
  for (auto c : control->GetChildren()) _TraverseDescendants(c, predicate);
}
bool Control::RequestFocus() {
  auto window = GetWindow();
  if (window == nullptr) return false;

  return window->RequestFocusFor(this);
}

bool Control::HasFocus() {
  auto window = GetWindow();
  if (window == nullptr) return false;

  return window->GetFocusControl() == this;
}

void Control::OnParentChanged(Control* old_parent, Control* new_parent) {}

void Control::OnAttachToWindow(Window* window) {}

void Control::OnDetachToWindow(Window* window) {}

void Control::OnMouseClickBegin(platform::native::MouseButton button) {}

void Control::OnMouseClickEnd(platform::native::MouseButton button) {}

void Control::OnMouseClickCancel(platform::native::MouseButton button) {}
}  // namespace cru::ui
