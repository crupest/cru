#include "cru/ui/controls/button.hpp"
#include <memory>

#include "cru/platform/graph/brush.hpp"
#include "cru/platform/graph/graph_factory.hpp"
#include "cru/platform/native/cursor.hpp"
#include "cru/platform/native/native_window.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/ui/render/border_render_object.hpp"
#include "cru/ui/ui_manager.hpp"
#include "cru/ui/window.hpp"

namespace cru::ui::controls {
using platform::native::GetSystemCursor;
using platform::native::SystemCursor;

namespace {
void Set(render::BorderRenderObject* o, const ButtonStateStyle& s) {
  o->SetBorderBrush(s.border_brush);
  o->SetBorderThickness(s.border_thickness);
  o->SetBorderRadius(s.border_radius);
  o->SetForegroundBrush(s.foreground_brush);
  o->SetBackgroundBrush(s.background_brush);
}
}  // namespace

Button::Button() : click_detector_(this) {
  style_ = UiManager::GetInstance()->GetThemeResources()->button_style;

  render_object_.reset(new render::BorderRenderObject);
  render_object_->SetAttachedControl(this);
  Set(render_object_.get(), style_.normal);
  render_object_->SetBorderEnabled(true);

  MouseEnterEvent()->Direct()->AddHandler([this](event::MouseEventArgs& args) {
    if (click_detector_.GetPressingButton() & trigger_button_) {
      SetState(ButtonState::Press);
    } else {
      SetState(ButtonState::Hover);
    }
  });

  MouseLeaveEvent()->Direct()->AddHandler([this](event::MouseEventArgs& args) {
    if (click_detector_.GetPressingButton() & trigger_button_) {
      SetState(ButtonState::Normal);
    } else {
      SetState(ButtonState::PressCancel);
    }
  });

  click_detector_.ClickBeginEvent()->AddHandler([this](MouseButton button) {
    if (button & trigger_button_) {
      SetState(ButtonState::Press);
    }
  });

  click_detector_.ClickEndEvent()->AddHandler([this](MouseButton button) {
    if (button & trigger_button_) {
      SetState(ButtonState::Hover);
    }
  });
}

render::RenderObject* Button::GetRenderObject() const {
  return render_object_.get();
}

void Button::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child != nullptr) render_object_->RemoveChild(0);
  if (new_child != nullptr)
    render_object_->AddChild(new_child->GetRenderObject(), 0);
}

void Button::OnStateChange(ButtonState oldState, ButtonState newState) {
  switch (newState) {
    case ButtonState::Normal:
      Set(render_object_.get(), style_.normal);
      SetCursor(GetSystemCursor(SystemCursor::Arrow));
      break;
    case ButtonState::Hover:
      Set(render_object_.get(), style_.hover);
      SetCursor(GetSystemCursor(SystemCursor::Hand));
      break;
    case ButtonState::Press:
      Set(render_object_.get(), style_.press);
      SetCursor(GetSystemCursor(SystemCursor::Hand));
      break;
    case ButtonState::PressCancel:
      Set(render_object_.get(), style_.press_cancel);
      SetCursor(GetSystemCursor(SystemCursor::Arrow));
      break;
  }
  GetWindow()->GetNativeWindow()->Repaint();
}
}  // namespace cru::ui::controls
