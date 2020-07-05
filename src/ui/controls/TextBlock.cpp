#include "cru/ui/controls/TextBlock.hpp"

#include "TextControlService.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/render/CanvasRenderObject.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

namespace cru::ui::controls {
using render::CanvasRenderObject;
using render::StackLayoutRenderObject;
using render::TextRenderObject;

TextBlock::TextBlock() {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush, theme_resources->caret_brush);

  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextControlService<TextBlock>>(this);
  service_->SetEnabled(true);
}

TextBlock::~TextBlock() = default;

render::RenderObject* TextBlock::GetRenderObject() const {
  return text_render_object_.get();
}

std::string TextBlock::GetText() const {
  return text_render_object_->GetText();
}

void TextBlock::SetText(std::string text) {
  text_render_object_->SetText(std::move(text));
}

gsl::not_null<render::TextRenderObject*> TextBlock::GetTextRenderObject() {
  return text_render_object_.get();
}
}  // namespace cru::ui::controls
