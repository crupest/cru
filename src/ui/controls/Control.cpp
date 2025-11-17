#include "cru/ui/controls/Control.h"
#include "cru/base/log/Logger.h"
#include "cru/ui/controls/Window.h"

#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/style/StyleRuleSet.h"

#include <format>

namespace cru::ui::controls {
using platform::gui::ICursor;
using platform::gui::IUiApplication;
using platform::gui::SystemCursorType;

Control::Control() {
  style_rule_set_ = std::make_shared<style::StyleRuleSet>();
  style_rule_set_bind_ =
      std::make_unique<style::StyleRuleSetBind>(this, style_rule_set_);

  MouseEnterEvent()->Direct()->AddHandler(
      [this](events::MouseEventArgs&) { this->is_mouse_over_ = true; });

  MouseLeaveEvent()->Direct()->AddHandler(
      [this](events::MouseEventArgs&) { this->is_mouse_over_ = false; });
}

Control::~Control() {
  if (auto window = GetWindow()) {
    if (window->IsInEventHandling()) {
      CRU_LOG_TAG_WARN(
          "Better use delete later to delete control during event handling.");
    }
  }

  if (auto window = GetWindow()) {
    window->NotifyControlDestroyed(this);
  }
  RemoveFromParent();
}

std::string Control::GetDebugId() const {
  return std::format("{}({})", GetControlType(),
                     static_cast<const void*>(this));
}

Window* Control::GetWindow() {
  auto parent = this;
  while (parent) {
    if (auto window = dynamic_cast<Window*>(parent)) {
      return window;
    }
    parent = parent->GetParent();
  }
  return nullptr;
}

void Control::SetParent(Control* parent) {
  if (parent_ == parent) return;
  auto old_parent = parent_;
  parent_ = parent;
  OnParentChanged(old_parent, parent);
}

bool Control::HasAncestor(Control* control) {
  auto parent = this;
  while (parent) {
    if (parent == control) return true;
    parent = parent->GetParent();
  }
  return false;
}

void Control::RemoveFromParent() {
  if (parent_) {
    parent_->RemoveChild(this);
  }
}

controls::Control* Control::HitTest(const Point& point) {
  const auto render_object = GetRenderObject()->HitTest(point);
  if (render_object) {
    const auto control = render_object->GetAttachedControl();
    assert(control);
    return control;
  }
  return nullptr;
}

bool Control::HasFocus() {
  auto window = GetWindow();
  if (window == nullptr) return false;

  return window->GetFocusControl() == this;
}

bool Control::CaptureMouse() {
  auto window = GetWindow();
  if (window == nullptr) return false;

  return window->SetMouseCaptureControl(this);
}

void Control::SetFocus() {
  auto window = GetWindow();
  if (window == nullptr) return;

  window->SetFocusControl(this);
}

bool Control::ReleaseMouse() {
  auto window = GetWindow();
  if (window == nullptr) return false;
  if (window->GetMouseCaptureControl() != this) return false;
  return window->SetMouseCaptureControl(nullptr);
}

bool Control::IsMouseCaptured() {
  auto window = GetWindow();
  if (window == nullptr) return false;

  return window->GetMouseCaptureControl() == this;
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
  const auto window = GetWindow();
  if (window != nullptr) {
    window->UpdateCursor();
  }
}

std::shared_ptr<style::StyleRuleSet> Control::GetStyleRuleSet() {
  return style_rule_set_;
}
}  // namespace cru::ui::controls
