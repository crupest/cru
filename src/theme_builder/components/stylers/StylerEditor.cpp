#include "StylerEditor.h"
#include "BorderStylerEditor.h"
#include "CompoundStylerEditor.h"
#include "CursorStylerEditor.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
StylerEditor::StylerEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(&label_);
}

StylerEditor::~StylerEditor() { container_.RemoveFromParent(); }

std::unique_ptr<StylerEditor> CreateStylerEditor(ui::style::Styler* styler) {
  if (auto compound_styler = dynamic_cast<ui::style::CompoundStyler*>(styler)) {
    auto result = std::make_unique<CompoundStylerEditor>();
    result->SetValue(compound_styler);
    return result;
  } else if (auto border_styler =
                 dynamic_cast<ui::style::BorderStyler*>(styler)) {
    auto editor = std::make_unique<BorderStylerEditor>();
    editor->SetValue(border_styler);
    return editor;
  } else if (auto cursor_styler =
                 dynamic_cast<ui::style::CursorStyler*>(styler)) {
    auto editor = std::make_unique<CursorStylerEditor>();
    editor->SetValue(cursor_styler);
    return editor;
  } else {
    return nullptr;
  }
}
}  // namespace cru::theme_builder::components::stylers
