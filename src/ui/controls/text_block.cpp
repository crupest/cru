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

TextBlock::TextBlock()
    : root_render_object_(new StackLayoutRenderObject()),
      text_render_object_(),
      caret_render_object_(new CanvasRenderObject()) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush);

  root_render_object_->AddChild(text_render_object_.get(), 0);
  root_render_object_->AddChild(caret_render_object_.get(), 1);

  root_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);
  caret_render_object_->SetAttachedControl(this);

  caret_brush_ = theme_resources->caret_brush;

  service_ = std::make_unique<TextControlService<TextBlock>>(this);
  service_->SetEnabled(true);
}

TextBlock::~TextBlock() = default;

render::RenderObject* TextBlock::GetRenderObject() const {
  return root_render_object_.get();
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

render::CanvasRenderObject* TextBlock::GetCaretRenderObject() {
  return caret_render_object_.get();
}

platform::graph::IBrush* TextBlock::GetCaretBrush() {
  return caret_brush_.get();
}
}  // namespace cru::ui::controls
