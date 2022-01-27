#include "cru/ui/controls/TextBox.hpp"

#include "cru/ui/ThemeManager.hpp"
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
  auto theme_manager = ThemeManager::GetInstance();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_manager->GetResourceBrush(u"text.brush"),
      theme_manager->GetResourceFont(u"text.font"),
      theme_manager->GetResourceBrush(u"text.selection.brush"),
      theme_manager->GetResourceBrush(u"text.caret.brush"));
  text_render_object_->SetEditMode(true);

  border_render_object_->AddChild(scroll_render_object_.get(), 0);
  scroll_render_object_->AddChild(text_render_object_.get(), 0);

  border_render_object_->SetAttachedControl(this);
  scroll_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);
  text_render_object_->SetMinSize(Size{100, 0});
  text_render_object_->SetMeasureIncludingTrailingSpace(true);

  service_ = std::make_unique<TextHostControlService>(this);
  service_->SetEnabled(true);
  service_->SetEditable(true);

  border_render_object_->SetBorderEnabled(true);

  GetStyleRuleSet()->SetParent(
      theme_manager->GetResourceStyleRuleSet(u"textbox.style"));
}

TextBox::~TextBox() {}

render::RenderObject* TextBox::GetRenderObject() const {
  return border_render_object_.get();
}

bool TextBox::GetMultiLine() const { return service_->IsMultiLine(); }

void TextBox::SetMultiLine(bool value) { service_->SetMultiLine(value); }

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
