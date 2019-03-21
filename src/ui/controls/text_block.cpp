#include "text_block.hpp"

#include "ui/render/text_render_object.hpp"
#include "ui/ui_manager.hpp"

namespace cru::ui::controls {
using render::TextRenderObject;

TextBlock::TextBlock() {
  const auto predefined_resources =
      UiManager::GetInstance()->GetPredefineResources();
  render_object_ =
      new TextRenderObject(predefined_resources->text_block_text_brush,
                           predefined_resources->text_block_text_format,
                           predefined_resources->text_block_selection_brush);
}

TextBlock::~TextBlock() { delete render_object_; }

render::RenderObject* TextBlock::GetRenderObject() const {
  return render_object_;
}

String TextBlock::GetText() const { return render_object_->GetText(); }

void TextBlock::SetText(const String& text) { render_object_->SetText(text); }
}  // namespace cru::ui::controls
