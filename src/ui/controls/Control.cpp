#include "cru/ui/controls/Control.h"

#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/style/StyleRuleSet.h"

namespace cru::ui::controls {
using platform::gui::ICursor;
using platform::gui::IUiApplication;
using platform::gui::SystemCursorType;

Control::Control() {
  style_rule_set_ = std::make_shared<style::StyleRuleSet>();
  style_rule_set_bind_ =
      std::make_unique<style::StyleRuleSetBind>(this, style_rule_set_);

  MouseEnterEvent()->Direct()->AddHandler([this](events::MouseEventArgs&) {
    this->is_mouse_over_ = true;
    this->OnMouseHoverChange(true);
  });

  MouseLeaveEvent()->Direct()->AddHandler([this](events::MouseEventArgs&) {
    this->is_mouse_over_ = false;
    this->OnMouseHoverChange(true);
  });
}

Control::~Control() {
  if (host::WindowHost::IsInEventHandling()) {
    // Don't delete control during event handling. Use DeleteLater.
    std::terminate();
  }

  in_destruction_ = true;
  RemoveFromParent();
}

void Control::SetParent(Control* parent) {
  if (parent_ == parent) return;
  auto old_parent = parent_;
  parent_ = parent;
  OnParentChangedCore(old_parent, parent);
}

void Control::RemoveFromParent() {
  if (parent_) {
    parent_->RemoveChild(this);
  }
}

bool Control::HasFocus() {
  auto host = GetWindowHost();
  if (host == nullptr) return false;

  return host->GetFocusControl() == this;
}

bool Control::CaptureMouse() {
  auto host = GetWindowHost();
  if (host == nullptr) return false;

  return host->CaptureMouseFor(this);
}

void Control::SetFocus() {
  auto host = GetWindowHost();
  if (host == nullptr) return;

  host->SetFocusControl(this);
}

bool Control::ReleaseMouse() {
  auto host = GetWindowHost();
  if (host == nullptr) return false;

  return host->CaptureMouseFor(nullptr);
}

bool Control::IsMouseCaptured() {
  auto host = GetWindowHost();
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
  const auto host = GetWindowHost();
  if (host != nullptr) {
    host->UpdateCursor();
  }
}

std::shared_ptr<style::StyleRuleSet> Control::GetStyleRuleSet() {
  return style_rule_set_;
}

void Control::OnParentChangedCore(Control* old_parent, Control* new_parent) {
  auto new_window_host =
      new_parent == nullptr ? nullptr : new_parent->GetWindowHost();
  if (window_host_ != new_window_host) {
    auto old_host = window_host_;
    window_host_ = new_window_host;
    OnWindowHostChangedCore(old_host, new_window_host);
  }

  if (!in_destruction_) OnParentChanged(old_parent, new_parent);
}

void Control::OnWindowHostChangedCore(host::WindowHost* old_host,
                                      host::WindowHost* new_host) {
  if (old_host != nullptr) {
    old_host->OnControlDetach(this);
  }

  if (!in_destruction_) {
    ForEachChild([old_host, new_host](Control* child) {
      child->window_host_ = new_host;
      child->OnWindowHostChangedCore(old_host, new_host);
    });
    OnWindowHostChanged(old_host, new_host);
  }
}

void Control::OnPrepareDelete() { RemoveFromParent(); }
}  // namespace cru::ui::controls
