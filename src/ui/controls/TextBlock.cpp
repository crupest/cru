#include "cru/ui/controls/TextBlock.h"

#include "../Helper.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/render/CanvasRenderObject.h"
#include "cru/ui/render/StackLayoutRenderObject.h"
#include "cru/ui/render/TextRenderObject.h"

namespace cru::ui::controls {
using render::TextRenderObject;

TextBlock::TextBlock() {
  const auto theme_manager = ThemeManager::GetInstance();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_manager->GetResourceBrush(u"text.brush"),
      theme_manager->GetResourceFont(u"text.font"),
      theme_manager->GetResourceBrush(u"text.selection.brush"),
      theme_manager->GetResourceBrush(u"text.caret.brush"));

  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextHostControlService>(this);

  service_->SetEnabled(false);
  service_->SetEditable(false);
}

TextBlock::~TextBlock() = default;

render::RenderObject* TextBlock::GetRenderObject() const {
  return text_render_object_.get();
}

String TextBlock::GetText() const { return service_->GetText(); }

void TextBlock::SetText(String text) { service_->SetText(std::move(text)); }

bool TextBlock::IsSelectable() const { return service_->IsEnabled(); }

void TextBlock::SetSelectable(bool value) { service_->SetEnabled(value); }

void TextBlock::SetTextColor(const Color& color) {
  text_render_object_->SetBrush(
      GetUiApplication()->GetGraphicsFactory()->CreateSolidColorBrush(color));
}

render::TextRenderObject* TextBlock::GetTextRenderObject() {
  return text_render_object_.get();
}
}  // namespace cru::ui::controls
