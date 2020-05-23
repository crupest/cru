#include "cru/ui/Control.hpp"

#include "cru/platform/native/Cursor.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/UiHost.hpp"
#include "RoutedEventDispatch.hpp"

#include <memory>

namespace cru::ui {
using platform::native::ICursor;
using platform::native::IUiApplication;
using platform::native::SystemCursorType;

Control::Control() {
  MouseEnterEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = true;
    this->OnMouseHoverChange(true);
  });

  MouseLeaveEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = false;
    this->OnMouseHoverChange(true);
  });
}

void Control::_SetParent(Control* parent) {
  const auto old_parent = GetParent();
  parent_ = parent;
  const auto new_parent = GetParent();
  if (old_parent != new_parent) OnParentChanged(old_parent, new_parent);
}

void Control::_SetDescendantUiHost(UiHost* host) {
  if (host == nullptr && ui_host_ == nullptr) return;

  // You can only attach or detach window.
  Expects((host != nullptr && ui_host_ == nullptr) ||
          (host == nullptr && ui_host_ != nullptr));

  if (host == nullptr) {
    const auto old = ui_host_;
    TraverseDescendants([old](Control* control) {
      control->ui_host_ = nullptr;
      control->OnDetachFromHost(old);
    });
  } else
    TraverseDescendants([host](Control* control) {
      control->ui_host_ = host;
      control->OnAttachToHost(host);
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
  auto host = GetUiHost();
  if (host == nullptr) return false;

  return host->RequestFocusFor(this);
}

bool Control::HasFocus() {
  auto host = GetUiHost();
  if (host == nullptr) return false;

  return host->GetFocusControl() == this;
}

bool Control::CaptureMouse() {
  auto host = GetUiHost();
  if (host == nullptr) return false;

  return host->CaptureMouseFor(this);
}

bool Control::ReleaseMouse() {
  auto host = GetUiHost();
  if (host == nullptr) return false;

  return host->CaptureMouseFor(nullptr);
}

bool Control::IsMouseCaptured() {
  auto host = GetUiHost();
  if (host == nullptr) return false;

  return host->GetMouseCaptureControl() == this;
}

std::shared_ptr<ICursor> Control::GetCursor() { return cursor_; }

std::shared_ptr<ICursor> Control::GetInheritedCursor() {
  Control* control = this;
  while (control != nullptr) {
    const auto cursor = control->GetCursor();
    if (cursor != nullptr) return cursor;
    control = control->GetParent();
  }
  return IUiApplication::GetInstance()->GetCursorManager()->GetSystemCursor(
      SystemCursorType::Arrow);
}

void Control::SetCursor(std::shared_ptr<ICursor> cursor) {
  cursor_ = std::move(cursor);
  const auto host = GetUiHost();
  if (host != nullptr) {
    host->UpdateCursor();
  }
}

void Control::OnParentChanged(Control* old_parent, Control* new_parent) {
  CRU_UNUSED(old_parent)
  CRU_UNUSED(new_parent)
}

void Control::OnAttachToHost(UiHost* host) { CRU_UNUSED(host) }

void Control::OnDetachFromHost(UiHost* host) { CRU_UNUSED(host) }
}  // namespace cru::ui
