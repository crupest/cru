#include "PaddingStylerEditor.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
PaddingStylerEditor::PaddingStylerEditor() {
  SetLabel(u"Padding Styler");
  GetContainer()->AddChild(thickness_editor_.GetRootControl());

  thickness_editor_.SetLabel(u"Thickness");

  ConnectChangeEvent(thickness_editor_);
}

PaddingStylerEditor::~PaddingStylerEditor() {}

ClonablePtr<ui::style::PaddingStyler> PaddingStylerEditor::GetValue() {
  return ui::style::PaddingStyler::Create(thickness_editor_.GetValue());
}

void PaddingStylerEditor::SetValue(ui::style::PaddingStyler* styler,
                                   bool trigger_change) {
  thickness_editor_.SetValue(styler->GetPadding(), false);

  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::stylers
