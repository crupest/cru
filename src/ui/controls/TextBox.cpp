#include "cru/ui/controls/TextBox.hpp"

#include "TextControlService.hpp"
#include "cru/ui/UiManager.hpp"
#include "cru/ui/render/BorderRenderObject.hpp"
#include "cru/ui/render/CanvasRenderObject.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

namespace cru::ui::controls {
using render::BorderRenderObject;
using render::CanvasRenderObject;
using render::ScrollRenderObject;
using render::StackLayoutRenderObject;
using render::TextRenderObject;

TextBox::TextBox()
    : border_render_object_(new BorderRenderObject()),
      scroll_render_object_(new ScrollRenderObject()) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  border_style_ = theme_resources->text_box_border_style;

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush, theme_resources->caret_brush);

  border_render_object_->AddChild(scroll_render_object_.get(), 0);
  scroll_render_object_->AddChild(text_render_object_.get(), 0);

  border_render_object_->SetAttachedControl(this);
  scroll_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextControlService<TextBox>>(this);
  service_->SetEnabled(true);
  service_->SetCaretVisible(true);

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

render::RenderObject* TextBox::GetRenderObject() const {
  return border_render_object_.get();
}

gsl::not_null<render::TextRenderObject*> TextBox::GetTextRenderObject() {
  return text_render_object_.get();
}

render::ScrollRenderObject* TextBox::GetScrollRenderObject() {
  return scroll_render_object_.get();
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
