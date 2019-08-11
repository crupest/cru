#include "cru/ui/control.hpp"

#include "cru/platform/native/basic_types.hpp"
#include "cru/ui/base.hpp"
#include "cru/ui/event/ui_event.hpp"
#include "cru/ui/window.hpp"
#include "routed_event_dispatch.hpp"

#include <cassert>

namespace cru::ui {
Control::Control() {
  MouseEnterEvent()->Direct()->AddHandler(
      [this](event::MouseEventArgs&) { this->is_mouse_over_ = true; });

  MouseLeaveEvent()->Direct()->AddHandler(
      [this](event::MouseEventArgs&) { this->is_mouse_over_ = false; });
}

void Control::_SetParent(Control* parent) {
  const auto old_parent = GetParent();
  parent_ = parent;
  const auto new_parent = GetParent();
  if (old_parent != new_parent) OnParentChanged(old_parent, new_parent);
}

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
