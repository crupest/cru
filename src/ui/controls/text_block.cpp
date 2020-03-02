#include "cru/ui/controls/text_block.hpp"

#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_manager.hpp"

namespace cru::ui::controls {
using render::TextRenderObject;

TextBlock::TextBlock() {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();
  render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush);
  render_object_->SetAttachedControl(this);
}

TextBlock::~TextBlock() = default;

render::RenderObject* TextBlock::GetRenderObject() const {
  return render_object_.get();
}

std::string TextBlock::GetText() const { return render_object_->GetText(); }

void TextBlock::SetText(std::string text) {
  render_object_->SetText(std::move(text));
}
}  // namespace cru::ui::controls
