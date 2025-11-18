#include "cru/ui/controls/Control.h"
#include "cru/base/Base.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/ControlHost.h"
#include "cru/ui/style/StyleRuleSet.h"

#include <algorithm>
#include <format>

namespace cru::ui::controls {
using platform::gui::ICursor;
using platform::gui::IUiApplication;
using platform::gui::SystemCursorType;

Control::Control() {
  style_rule_set_ = std::make_shared<style::StyleRuleSet>();
  style_rule_set_bind_ =
      std::make_unique<style::StyleRuleSetBind>(this, style_rule_set_);
}

Control::~Control() {
  if (host_ && host_->IsInEventHandling()) {
    CRU_LOG_TAG_WARN(
        "Better use delete later to delete control during event handling.");
  }

  RemoveFromParent();
  RemoveAllChild();
}

std::string Control::GetDebugId() const {
  return std::format("{}({})", GetControlType(),
                     static_cast<const void*>(this));
}

ControlHost* Control::GetControlHost() { return host_; }

Control* Control::GetParent() { return parent_; }

bool Control::HasAncestor(Control* control) {
  auto parent = this;
  while (parent) {
    if (parent == control) return true;
    parent = parent->GetParent();
  }
  return false;
}

const std::vector<Control*>& Control::GetChildren() { return children_; }

void Control::RemoveChild(Control* child) {
  auto iter = std::ranges::find(children_, child);
  if (iter != children_.cend()) {
    RemoveChildAt(iter - children_.cbegin());
  }
}

void Control::RemoveAllChild() {
  while (!GetChildren().empty()) {
    RemoveChildAt(GetChildren().size() - 1);
  }
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

void Control::InsertChildAt(Control* control, Index index) {
  if (index < 0 || index > children_.size()) {
    throw Exception("Child control index out of range.");
  }

  if (control->parent_) {
    throw Exception("Control already has a parent.");
  }

  children_.insert(children_.cbegin() + index, control);
  control->parent_ = this;
  control->TraverseDescendents(
      [this](Control* control) { control->host_ = host_; }, true);
  if (host_) {
    host_->NotifyControlParentChange(control, nullptr, this);
  }
  control->OnParentChanged(nullptr, this);
  OnChildInserted(control, index);

  if (host_) {
    host_->InvalidateLayout();
  }
}

void Control::RemoveChildAt(Index index) {
  if (index < 0 || index >= children_.size()) {
    throw Exception("Child control index out of range.");
  }

  auto control = children_[index];
  children_.erase(children_.cbegin() + index);
  control->parent_ = nullptr;
  control->TraverseDescendents(
      [this](Control* control) { control->host_ = nullptr; }, true);
  if (host_) {
    host_->NotifyControlParentChange(control, this, nullptr);
  }
  control->OnParentChanged(this, nullptr);
  OnChildRemoved(control, index);

  if (host_) {
    host_->InvalidateLayout();
  }
}

void Control::AddChild(Control* control) {
  InsertChildAt(control, GetChildren().size());
}

bool Control::HasFocus() {
  if (!host_) return false;
  return host_->GetFocusControl() == this;
}

void Control::SetFocus() {
  if (!host_) return;
  host_->SetFocusControl(this);
}

bool Control::IsMouseOver() {
  if (!host_) return false;
  return host_->GetMouseHoverControl() == this;
}

bool Control::CaptureMouse() {
  if (!host_) return false;
  return host_->SetMouseCaptureControl(this);
}

bool Control::ReleaseMouse() {
  if (!host_) return false;
  if (!IsMouseCaptured()) return false;
  return host_->SetMouseCaptureControl(nullptr);
}

bool Control::IsMouseCaptured() {
  if (!host_) return false;
  return host_->GetMouseCaptureControl() == this;
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
  if (host_) {
    host_->UpdateCursor();
  }
}

std::shared_ptr<style::StyleRuleSet> Control::GetStyleRuleSet() {
  return style_rule_set_;
}

void Control::OnParentChanged(Control* old_parent, Control* new_parent) {}
void Control::OnChildInserted(Control* control, Index index) {}
void Control::OnChildRemoved(Control* control, Index index) {}
}  // namespace cru::ui::controls
