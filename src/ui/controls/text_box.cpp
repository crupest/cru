#include "cru/ui/controls/text_box.hpp"

#include "cru/ui/render/border_render_object.hpp"
#include "cru/ui/render/canvas_render_object.hpp"
#include "cru/ui/render/stack_layout_render_object.hpp"
#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_manager.hpp"
#include "text_control_service.hpp"

namespace cru::ui::controls {
using render::BorderRenderObject;
using render::CanvasRenderObject;
using render::StackLayoutRenderObject;
using render::TextRenderObject;

TextBox::TextBox()
    : border_render_object_(new BorderRenderObject()),
      stack_layout_render_object_(new StackLayoutRenderObject()),
      text_render_object_(),
      caret_render_object_(new CanvasRenderObject()),
      service_(new TextControlService<TextBox>(this)) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  caret_brush_ = theme_resources->caret_brush;
  border_style_ = theme_resources->text_box_border_style;

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush);

  border_render_object_->AddChild(stack_layout_render_object_.get(), 0);
  stack_layout_render_object_->AddChild(text_render_object_.get(), 0);
  stack_layout_render_object_->AddChild(caret_render_object_.get(), 1);

  border_render_object_->SetAttachedControl(this);
  stack_layout_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);
  caret_render_object_->SetAttachedControl(this);
}

TextBox::~TextBox() {}

render::TextRenderObject* TextBox::GetTextRenderObject() {
  return text_render_object_.get();
}

render::CanvasRenderObject* TextBox::GetCaretRenderObject() {
  return caret_render_object_.get();
}

std::shared_ptr<platform::graph::IBrush> TextBox::GetCaretBrush() {
  return caret_brush_;
}

const TextBoxBorderStyle& TextBox::GetBorderStyle() { return border_style_; }

void TextBox::SetBorderStyle(TextBoxBorderStyle border_style) {
  border_style_ = std::move(border_style_);
}
}  // namespace cru::ui::controls
