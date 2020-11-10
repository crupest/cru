#include "cru/ui/controls/TextBlock.hpp"

#include "TextControlService.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/render/CanvasRenderObject.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

namespace cru::ui::controls {
using render::TextRenderObject;

TextBlock* TextBlock::Create() { return new TextBlock(); }

TextBlock* TextBlock::Create(std::u16string text, bool selectable) {
  auto c = new TextBlock();
  c->SetText(text);
  c->SetSelectable(selectable);
  return c;
}

TextBlock::TextBlock() {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush, theme_resources->caret_brush);

  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextControlService<TextBlock>>(this);

  service_->SetEnabled(false);
  service_->SetEditable(false);
}

TextBlock::~TextBlock() = default;

render::RenderObject* TextBlock::GetRenderObject() const {
  return text_render_object_.get();
}

std::u16string TextBlock::GetText() const { return service_->GetText(); }

void TextBlock::SetText(std::u16string text) {
  service_->SetText(std::move(text));
}

bool TextBlock::IsSelectable() const { return service_->IsEnabled(); }

void TextBlock::SetSelectable(bool value) { service_->SetEnabled(value); }

gsl::not_null<render::TextRenderObject*> TextBlock::GetTextRenderObject() {
  return text_render_object_.get();
}
}  // namespace cru::ui::controls
