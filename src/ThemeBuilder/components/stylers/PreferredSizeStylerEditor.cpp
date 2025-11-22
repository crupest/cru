#include "PreferredSizeStylerEditor.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
PreferredSizeStylerEditor::PreferredSizeStylerEditor() {
  SetLabel("Preferred Size Styler");
  GetContainer()->AddChild(width_editor_.GetRootControl());
  GetContainer()->AddChild(height_editor_.GetRootControl());

  width_editor_.SetLabel("Width");
  height_editor_.SetLabel("Height");

  ConnectChangeEvent(width_editor_);
  ConnectChangeEvent(height_editor_);
}

PreferredSizeStylerEditor::~PreferredSizeStylerEditor() {}

ClonePtr<ui::style::PreferredSizeStyler>
PreferredSizeStylerEditor::GetValue() {
  return ui::style::PreferredSizeStyler::Create(ui::render::MeasureSize{
      width_editor_.GetValue(), height_editor_.GetValue()});
}

void PreferredSizeStylerEditor::SetValue(ui::style::PreferredSizeStyler* styler,
                                         bool trigger_change) {
  width_editor_.SetValue(styler->GetSuggestSize().width, false);
  height_editor_.SetValue(styler->GetSuggestSize().height, false);

  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::stylers
