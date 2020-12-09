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
using render::ScrollRenderObject;
using render::TextRenderObject;

TextBox::TextBox()
    : border_render_object_(new BorderRenderObject()),
      scroll_render_object_(new ScrollRenderObject()) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush, theme_resources->caret_brush);

  border_render_object_->AddChild(scroll_render_object_.get(), 0);
  scroll_render_object_->AddChild(text_render_object_.get(), 0);

  border_render_object_->SetAttachedControl(this);
  scroll_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);
  text_render_object_->SetMinSize(Size{100, 24});

  service_ = std::make_unique<TextControlService<TextBox>>(this);
  service_->SetEnabled(true);
  service_->SetEditable(true);

  border_render_object_->SetBorderEnabled(true);

  GetStyleRuleSet()->SetParent(&theme_resources->text_box_style);
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

void TextBox::ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) {
  border_render_object_->ApplyBorderStyle(style);
}
}  // namespace cru::ui::controls
