#include "FontStylerEditor.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
FontStylerEditor::FontStylerEditor() {
  GetContainer()->AddChild(font_editor_.GetRootControl());

  ConnectChangeEvent(font_editor_);
}

FontStylerEditor::~FontStylerEditor() {}

ClonablePtr<ui::style::FontStyler> FontStylerEditor::GetValue() const {
  return ui::style::FontStyler::Create(font_editor_.GetValue());
}

void FontStylerEditor::SetValue(ui::style::FontStyler* value,
                                bool trigger_change) {
  font_editor_.SetValue(value->GetFont(), trigger_change);
}
}  // namespace cru::theme_builder::components::stylers
