#include "cru/ui/controls/Control.h"

#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/render/RenderObject.h"
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

Control::~Control() { ReleaseMouse(); }

host::WindowHost* Control::GetWindowHost() const {
  auto parent = GetParent();
  if (parent) {
    return parent->GetWindowHost();
  }
  return nullptr;
}

void Control::SetParent(Control* parent) {
  if (parent_ == parent) return;
  auto old_parent = parent_;
  parent_ = parent;
  OnParentChanged(old_parent, parent);
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
}  // namespace cru::ui::controls
