#include "StylerEditor.h"
#include "../Common.h"
#include "BorderStylerEditor.h"
#include "CompoundStylerEditor.h"
#include "ContentBrushStylerEditor.h"
#include "CursorStylerEditor.h"
#include "FontStylerEditor.h"
#include "MarginStylerEditor.h"
#include "PaddingStylerEditor.h"
#include "PreferredSizeStylerEditor.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/render/FlexLayoutRenderObject.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
StylerEditor::StylerEditor() {}

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
  } else if (auto content_brush_styler =
                 dynamic_cast<ui::style::ContentBrushStyler*>(styler)) {
    auto editor = std::make_unique<ContentBrushStylerEditor>();
    editor->SetValue(content_brush_styler);
    return editor;
  } else if (auto font_styler = dynamic_cast<ui::style::FontStyler*>(styler)) {
    auto editor = std::make_unique<FontStylerEditor>();
    editor->SetValue(font_styler);
    return editor;
  } else {
    throw Exception("Unknown styler type");
  }
}
}  // namespace cru::theme_builder::components::stylers
