#include "cru/ui/controls/Control.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/RenderObject.hpp"
#include "cru/ui/style/StyleRuleSet.hpp"

#include <memory>

namespace cru::ui::controls {
using platform::gui::ICursor;
using platform::gui::IUiApplication;
using platform::gui::SystemCursorType;

Control::Control() {
  style_rule_set_ = std::make_unique<style::StyleRuleSet>(this);

  MouseEnterEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = true;
    this->OnMouseHoverChange(true);
  });

  MouseLeaveEvent()->Direct()->AddHandler([this](event::MouseEventArgs&) {
    this->is_mouse_over_ = false;
    this->OnMouseHoverChange(true);
  });
}

Control::~Control() {
  for (const auto child : children_) delete child;
}

host::WindowHost* Control::GetWindowHost() const { return window_host_; }

void Control::TraverseDescendants(
    const std::function<void(Control*)>& predicate) {
  predicate(this);
  for (auto c : GetChildren()) c->TraverseDescendants(predicate);
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

style::StyleRuleSet* Control::GetStyleRuleSet() {
  return style_rule_set_.get();
}

void Control::AddChild(Control* control, const Index position) {
  Expects(control->GetParent() ==
          nullptr);  // The control already has a parent.
  Expects(position >= 0);
  Expects(position <= static_cast<Index>(
                          children_.size()));  // The position is out of range.

  children_.insert(children_.cbegin() + position, control);

  const auto old_parent = control->parent_;
  control->parent_ = this;

  OnAddChild(control, position);
  control->OnParentChanged(old_parent, this);

  if (window_host_)
    control->TraverseDescendants([this](Control* control) {
      control->window_host_ = window_host_;
      control->OnAttachToHost(window_host_);
    });
}

void Control::RemoveChild(const Index position) {
  Expects(position >= 0);
  Expects(position < static_cast<Index>(
                         children_.size()));  // The position is out of range.

  const auto i = children_.cbegin() + position;
  const auto control = *i;

  children_.erase(i);
  control->parent_ = nullptr;

  OnRemoveChild(control, position);
  control->OnParentChanged(this, nullptr);

  if (window_host_)
    control->TraverseDescendants([this](Control* control) {
      control->window_host_ = nullptr;
      control->OnDetachFromHost(window_host_);
    });
}

void Control::OnAddChild(Control* child, Index position) {
  CRU_UNUSED(child)
  CRU_UNUSED(position)
}
void Control::OnRemoveChild(Control* child, Index position) {
  CRU_UNUSED(child)
  CRU_UNUSED(position)
}

void Control::OnParentChanged(Control* old_parent, Control* new_parent) {
  CRU_UNUSED(old_parent)
  CRU_UNUSED(new_parent)
}

void Control::OnAttachToHost(host::WindowHost* host) { CRU_UNUSED(host) }

void Control::OnDetachFromHost(host::WindowHost* host) { CRU_UNUSED(host) }
}  // namespace cru::ui::controls
