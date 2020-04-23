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

TextBox::TextBox() : border_render_object_(new BorderRenderObject()) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  border_style_ = theme_resources->text_box_border_style;

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush, theme_resources->caret_brush);

  border_render_object_->AddChild(text_render_object_.get(), 0);

  border_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextControlService<TextBox>>(this);
  service_->SetEnabled(true);

  GainFocusEvent()->Direct()->AddHandler([this](event::FocusChangeEventArgs&) {
    this->service_->SetEnabled(true);
    this->UpdateBorderStyle();
  });

  LoseFocusEvent()->Direct()->AddHandler([this](event::FocusChangeEventArgs&) {
    this->service_->SetEnabled(false);
    this->UpdateBorderStyle();
  });
}

TextBox::~TextBox() {}

render::TextRenderObject* TextBox::GetTextRenderObject() {
  return text_render_object_.get();
}

const TextBoxBorderStyle& TextBox::GetBorderStyle() { return border_style_; }

void TextBox::SetBorderStyle(TextBoxBorderStyle border_style) {
  border_style_ = std::move(border_style);
}

void TextBox::OnMouseHoverChange(bool) { UpdateBorderStyle(); }

void TextBox::UpdateBorderStyle() {
  const auto focus = HasFocus();
  const auto hover = IsMouseOver();
  border_render_object_->SetBorderStyle(
      focus ? (hover ? border_style_.focus_hover : border_style_.focus)
            : (hover ? border_style_.hover : border_style_.normal));
}
}  // namespace cru::ui::controls
