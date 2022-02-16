#include "BorderStylerEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
BorderStylerEditor::BorderStylerEditor() {
  GetContainer()->AddChild(corner_radius_editor_.GetRootControl());
  GetContainer()->AddChild(thickness_editor_.GetRootControl());
  GetContainer()->AddChild(brush_editor_.GetRootControl());
  GetContainer()->AddChild(foreground_brush_editor_.GetRootControl());
  GetContainer()->AddChild(background_brush_editor_.GetRootControl());

  auto connect = [this](IEvent<std::nullptr_t>* event) {
    event->AddHandler(
        [this](std::nullptr_t) { this->change_event_.Raise(nullptr); });
  };

  connect(corner_radius_editor_.ChangeEvent());
  connect(thickness_editor_.ChangeEvent());
  connect(brush_editor_.ChangeEvent());
  connect(foreground_brush_editor_.ChangeEvent());
  connect(background_brush_editor_.ChangeEvent());
}

BorderStylerEditor::~BorderStylerEditor() {}

ClonablePtr<ui::style::BorderStyler> BorderStylerEditor::GetValue() {
  auto graphics_factory =
      platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory();

  ui::style::ApplyBorderStyleInfo border_style;
  border_style.border_radius = corner_radius_editor_.GetValue();
  border_style.border_thickness = thickness_editor_.GetValue();

  if (brush_editor_.IsEnabled()) {
    border_style.border_brush = graphics_factory->CreateSolidColorBrush(
        brush_editor_.GetEditor()->GetValue());
  }

  if (foreground_brush_editor_.IsEnabled()) {
    border_style.foreground_brush = graphics_factory->CreateSolidColorBrush(
        foreground_brush_editor_.GetEditor()->GetValue());
  }

  if (background_brush_editor_.IsEnabled()) {
    border_style.background_brush = graphics_factory->CreateSolidColorBrush(
        background_brush_editor_.GetEditor()->GetValue());
  }

  return ui::style::BorderStyler::Create(border_style);
}

void BorderStylerEditor::SetValue(ui::style::BorderStyler* styler,
                                  bool trigger_change) {
  Expects(styler);

  auto border_style = styler->GetBorderStyle();
  corner_radius_editor_.SetValue(border_style.border_radius, false);
  thickness_editor_.SetValue(border_style.border_thickness, false);

  brush_editor_.SetEnabled(border_style.border_brush.has_value(), false);
  if (border_style.border_brush.has_value()) {
    brush_editor_.GetEditor()->SetValue(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.border_brush.value())
            ->GetColor(),
        false);
  }

  foreground_brush_editor_.SetEnabled(border_style.foreground_brush.has_value(),
                                      false);
  if (border_style.foreground_brush.has_value()) {
    foreground_brush_editor_.GetEditor()->SetValue(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.foreground_brush.value())
            ->GetColor(),
        false);
  }

  background_brush_editor_.SetEnabled(border_style.background_brush.has_value(),
                                      false);
  if (border_style.background_brush.has_value()) {
    background_brush_editor_.GetEditor()->SetValue(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.background_brush.value())
            ->GetColor(),
        false);
  }

  if (trigger_change) {
    change_event_.Raise(nullptr);
  }
}

}  // namespace cru::theme_builder::components::stylers
