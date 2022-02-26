#include "StylerEditor.h"
#include "BorderStylerEditor.h"
#include "CompoundStylerEditor.h"
#include "CursorStylerEditor.h"
#include "MarginStylerEditor.h"
#include "PaddingStylerEditor.h"
#include "PreferredSizeStylerEditor.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/render/FlexLayoutRenderObject.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
StylerEditor::StylerEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(&head_container_);
  container_.SetItemCrossAlign(ui::controls::FlexCrossAlignment::Start);
  head_container_.SetFlexDirection(ui::render::FlexDirection::Horizontal);
  head_container_.AddChild(&label_);
}

StylerEditor::~StylerEditor() {}

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
  } else if (auto preferred_size_styler =
                 dynamic_cast<ui::style::PreferredSizeStyler*>(styler)) {
    auto editor = std::make_unique<PreferredSizeStylerEditor>();
    editor->SetValue(preferred_size_styler);
    return editor;
  } else if (auto margin_styler =
                 dynamic_cast<ui::style::MarginStyler*>(styler)) {
    auto editor = std::make_unique<MarginStylerEditor>();
    editor->SetValue(margin_styler);
    return editor;
  } else if (auto padding_styler =
                 dynamic_cast<ui::style::PaddingStyler*>(styler)) {
    auto editor = std::make_unique<PaddingStylerEditor>();
    editor->SetValue(padding_styler);
    return editor;
  } else {
    throw Exception(u"Unknown styler type");
  }
}
}  // namespace cru::theme_builder::components::stylers
