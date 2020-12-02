#include "cru/ui/controls/Button.hpp"
#include <memory>

#include "../Helper.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/render/BorderRenderObject.hpp"

namespace cru::ui::controls {
using cru::platform::gui::SystemCursorType;

namespace {
void Set(render::BorderRenderObject* o, const ButtonStateStyle& s) {
  o->SetBorderBrush(s.border_brush);
  o->SetBorderThickness(s.border_thickness);
  o->SetBorderRadius(s.border_radius);
  o->SetForegroundBrush(s.foreground_brush);
  o->SetBackgroundBrush(s.background_brush);
}

std::shared_ptr<platform::gui::ICursor> GetSystemCursor(SystemCursorType type) {
  return GetUiApplication()->GetCursorManager()->GetSystemCursor(type);
}
}  // namespace

Button::Button() : click_detector_(this) {
  style_ = UiManager::GetInstance()->GetThemeResources()->button_style;

  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());

  Set(render_object_.get(), style_.normal);
  render_object_->SetBorderEnabled(true);

  click_detector_.StateChangeEvent()->AddHandler(
      [this](const helper::ClickState& state) {
        switch (state) {
          case helper::ClickState::None:
            Set(render_object_.get(), style_.normal);
            SetCursor(GetSystemCursor(SystemCursorType::Arrow));
            break;
          case helper::ClickState::Hover:
            Set(render_object_.get(), style_.hover);
            SetCursor(GetSystemCursor(SystemCursorType::Hand));
            break;
          case helper::ClickState::Press:
            Set(render_object_.get(), style_.press);
            SetCursor(GetSystemCursor(SystemCursorType::Hand));
            break;
          case helper::ClickState::PressInactive:
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

void Button::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  render_object_->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
