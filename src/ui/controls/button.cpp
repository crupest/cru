#include "cru/ui/controls/Button.hpp"
#include <memory>

#include "../Helper.hpp"
#include "cru/platform/graph/Brush.hpp"
#include "cru/platform/native/Cursor.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/ui/render/BorderRenderObject.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/Window.hpp"

namespace cru::ui::controls {
using cru::platform::native::SystemCursorType;

namespace {
void Set(render::BorderRenderObject* o, const ButtonStateStyle& s) {
  o->SetBorderBrush(s.border_brush);
  o->SetBorderThickness(s.border_thickness);
  o->SetBorderRadius(s.border_radius);
  o->SetForegroundBrush(s.foreground_brush);
  o->SetBackgroundBrush(s.background_brush);
}

std::shared_ptr<platform::native::ICursor> GetSystemCursor(
    SystemCursorType type) {
  return GetUiApplication()->GetCursorManager()->GetSystemCursor(type);
}
}  // namespace

Button::Button() : click_detector_(this) {
  style_ = UiManager::GetInstance()->GetThemeResources()->button_style;

  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetAttachedControl(this);
  Set(render_object_.get(), style_.normal);
  render_object_->SetBorderEnabled(true);

  click_detector_.StateChangeEvent()->AddHandler(
      [this](const ClickState& state) {
        switch (state) {
          case ClickState::None:
            Set(render_object_.get(), style_.normal);
            SetCursor(GetSystemCursor(SystemCursorType::Arrow));
            break;
          case ClickState::Hover:
            Set(render_object_.get(), style_.hover);
            SetCursor(GetSystemCursor(SystemCursorType::Hand));
            break;
          case ClickState::Press:
            Set(render_object_.get(), style_.press);
            SetCursor(GetSystemCursor(SystemCursorType::Hand));
            break;
          case ClickState::PressInactive:
            Set(render_object_.get(), style_.press_cancel);
            SetCursor(GetSystemCursor(SystemCursorType::Arrow));
            break;
        }
      });
}

Button::~Button() = default;

render::RenderObject* Button::GetRenderObject() const {
  return render_object_.get();
}

void Button::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child != nullptr) render_object_->RemoveChild(0);
  if (new_child != nullptr)
    render_object_->AddChild(new_child->GetRenderObject(), 0);
}

}  // namespace cru::ui::controls
