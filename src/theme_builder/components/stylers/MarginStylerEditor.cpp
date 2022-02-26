#include "MarginStylerEditor.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
MarginStylerEditor::MarginStylerEditor() {
  SetLabel(u"Margin Styler");
  GetContainer()->AddChild(thickness_editor_.GetRootControl());

  thickness_editor_.SetLabel(u"Thickness");

  ConnectChangeEvent(thickness_editor_);
}

MarginStylerEditor::~MarginStylerEditor() {}

ClonablePtr<ui::style::MarginStyler> MarginStylerEditor::GetValue() {
  return ui::style::MarginStyler::Create(thickness_editor_.GetValue());
}

void MarginStylerEditor::SetValue(ui::style::MarginStyler* styler,
                                  bool trigger_change) {
  thickness_editor_.SetValue(styler->GetMargin(), false);

  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::stylers
