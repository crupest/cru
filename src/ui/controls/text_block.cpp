#include "cru/ui/controls/text_block.hpp"

#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_manager.hpp"

namespace cru::ui::controls {
using render::TextRenderObject;

TextBlock::TextBlock() {
  const auto predefined_resources =
      UiManager::GetInstance()->GetPredefineResources();
  render_object_.reset(
      new TextRenderObject(predefined_resources->text_block_text_brush,
                           predefined_resources->text_block_font,
                           predefined_resources->text_block_selection_brush));
}

render::RenderObject* TextBlock::GetRenderObject() const {
  return render_object_.get();
}

std::wstring TextBlock::GetText() const { return render_object_->GetText(); }

void TextBlock::SetText(std::wstring text) {
  render_object_->SetText(std::move(text));
}
}  // namespace cru::ui::controls
