#include "BorderStylerEditor.h"
#include <memory>
#include "cru/common/ClonablePtr.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::styler {
BorderStylerEditor::BorderStylerEditor() {
  container_.AddChild(corner_radius_editor_.GetRootControl());
  container_.AddChild(thickness_editor_.GetRootControl());
  container_.AddChild(brush_editor_.GetRootControl());
  container_.AddChild(foreground_brush_editor_.GetRootControl());
  container_.AddChild(background_brush_editor_.GetRootControl());

  // TODO: Add change listener.
}

BorderStylerEditor::~BorderStylerEditor() { container_.RemoveFromParent(); }

ClonablePtr<ui::style::BorderStyler> BorderStylerEditor::GetStyler() {
  auto graphics_factory =
      platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory();

  ui::style::ApplyBorderStyleInfo border_style;
  if (corner_radius_editor_.IsEnabled()) {
    border_style.border_radius =
        corner_radius_editor_.GetEditor()->GetCornerRadius();
  }

  if (thickness_editor_.IsEnabled()) {
    border_style.border_thickness =
        thickness_editor_.GetEditor()->GetThickness();
  }

  if (brush_editor_.IsEnabled()) {
    border_style.border_brush = graphics_factory->CreateSolidColorBrush(
        brush_editor_.GetEditor()->GetColor());
  }

  if (foreground_brush_editor_.IsEnabled()) {
    border_style.foreground_brush = graphics_factory->CreateSolidColorBrush(
        foreground_brush_editor_.GetEditor()->GetColor());
  }

  if (background_brush_editor_.IsEnabled()) {
    border_style.background_brush = graphics_factory->CreateSolidColorBrush(
        background_brush_editor_.GetEditor()->GetColor());
  }

  return ui::style::BorderStyler::Create(border_style);
}

void BorderStylerEditor::SetStyler(
    const ClonablePtr<ui::style::BorderStyler> &styler) {
  Expects(styler);

  auto border_style = styler->GetBorderStyle();
  corner_radius_editor_.SetEnabled(border_style.border_radius.has_value());
  if (border_style.border_radius.has_value()) {
    corner_radius_editor_.GetEditor()->SetCornerRadius(
        *border_style.border_radius);
  }

  thickness_editor_.SetEnabled(border_style.border_thickness.has_value());
  if (border_style.border_thickness.has_value()) {
    thickness_editor_.GetEditor()->SetThickness(*border_style.border_thickness);
  }

  brush_editor_.SetEnabled(border_style.border_brush.has_value());
  if (border_style.border_brush.has_value()) {
    brush_editor_.GetEditor()->SetColor(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.border_brush.value())
            ->GetColor());
  }

  foreground_brush_editor_.SetEnabled(
      border_style.foreground_brush.has_value());
  if (border_style.foreground_brush.has_value()) {
    foreground_brush_editor_.GetEditor()->SetColor(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.foreground_brush.value())
            ->GetColor());
  }

  background_brush_editor_.SetEnabled(
      border_style.background_brush.has_value());

  if (border_style.background_brush.has_value()) {
    background_brush_editor_.GetEditor()->SetColor(
        std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
            border_style.background_brush.value())
            ->GetColor());
  }
}

}  // namespace cru::theme_builder::components::styler
