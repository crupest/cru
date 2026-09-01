#include "cru/ui/controls/Control.h"
#include "cru/base/Base.h"
#include "cru/base/TreeObject.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/ControlHost.h"
#include "cru/ui/render/RenderObject.h"
#include "cru/ui/style/StyleRuleSet.h"

#include <format>

namespace cru::ui::controls {
using platform::gui::ICursor;
using platform::gui::IUiApplication;
using platform::gui::SystemCursorType;

Control::Control(std::string name, render::RenderObject* root_render_object)
    : name_(std::move(name)),
      host_(nullptr),
      root_render_object_(root_render_object) {
  style_rule_set_ = std::make_shared<style::StyleRuleSet>();
  style_rule_set_bind_ =
      std::make_unique<style::StyleRuleSetBind>(this, style_rule_set_);
}

Control::~Control() {
  if (host_ && host_->IsInEventHandling()) {
    CruLogWarn(
        kLogTag,
        "Better use delete later to delete control during event handling.");
  }

  DetachFromTree();
}

std::string Control::GetName() { return name_; }

std::string Control::GetDebugId() {
  return std::format("{}({})", GetName(), static_cast<const void*>(this));
}

ControlHost* Control::GetControlHost() { return host_; }

controls::Control* Control::HitTest(const Point& point) {
  const auto render_object = GetRenderObject()->HitTest(point);
  if (render_object) {
    const auto control = render_object->GetAttachedControl();
    assert(control);
    return control;
  }
  return nullptr;
}

Point Control::GetRenderObjectOffset(render::RenderObject* render_object) {
  Point offset{};
  auto this_render_object = GetRenderObject();
  while (render_object != this_render_object) {
    if (render_object == nullptr) {
      throw Exception("Render object is not a descendant of this control.");
    }

    offset += render_object->GetOffset();
    render_object = render_object->GetParent();
  }
  return offset;
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

void Control::OnChildInserted(Control* control, Index index) {
  assert(control->host_ == nullptr);
  if (host_) {
    control->TraverseDescendents(
        [this](Control* control) { control->host_ = this->host_; }, true);

    host_->NotifyControlParentChange(control, nullptr, this);

    control->TraverseDescendents(
        [this](Control* control) {
          control->ControlHostChangeEvent_.Raise({nullptr, this->host_});
        },
        true);
  }

  TreeObjectMixin::OnChildInserted(control, index);
}

void Control::OnChildRemoved(Control* control, Index index) {
  assert(control->host_ == host_);

  if (host_) {
    control->TraverseDescendents(
        [](Control* control) { control->host_ = nullptr; }, true);

    host_->NotifyControlParentChange(control, this, nullptr);

    control->TraverseDescendents(
        [this](Control* control) {
          control->ControlHostChangeEvent_.Raise({this->host_, nullptr});
        },
        true);
  }

  TreeObjectMixin::OnChildRemoved(control, index);
}
}  // namespace cru::ui::controls
