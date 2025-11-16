#include "ContentBrushStylerEditor.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
ContentBrushStylerEditor::ContentBrushStylerEditor() {
  GetContainer()->AddChild(color_editor_.GetRootControl());

  ConnectChangeEvent(color_editor_);
}

ContentBrushStylerEditor::~ContentBrushStylerEditor() {}

ClonePtr<ui::style::ContentBrushStyler> ContentBrushStylerEditor::GetValue()
    const {
  return ui::style::ContentBrushStyler::Create(
      platform::gui::IUiApplication::GetInstance()
          ->GetGraphicsFactory()
          ->CreateSolidColorBrush(color_editor_.GetValue()));
}

void ContentBrushStylerEditor::SetValue(ui::style::ContentBrushStyler* value,
                                        bool trigger_change) {
  color_editor_.SetValue(
      std::dynamic_pointer_cast<platform::graphics::ISolidColorBrush>(
          value->GetBrush())
          ->GetColor(),
      trigger_change);
}
}  // namespace cru::theme_builder::components::stylers
