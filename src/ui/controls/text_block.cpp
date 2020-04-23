#include "cru/ui/controls/text_block.hpp"

#include "cru/ui/render/canvas_render_object.hpp"
#include "cru/ui/render/stack_layout_render_object.hpp"
#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_manager.hpp"
#include "text_control_service.hpp"

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

render::TextRenderObject* TextBlock::GetTextRenderObject() {
  return text_render_object_.get();
}
}  // namespace cru::ui::controls
