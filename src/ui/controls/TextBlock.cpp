#include "cru/ui/controls/TextBlock.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/render/TextRenderObject.h"

#include <memory>

namespace cru::ui::controls {
TextBlock::TextBlock() : Control(kControlName) {
  auto theme_manager = ThemeManager::GetInstance();
  text_render_object_ = std::make_unique<render::TextRenderObject>(
      theme_manager->GetResourceBrush("text.brush"),
      theme_manager->GetResourceFont("text.font"),
      theme_manager->GetResourceBrush("text.selection.brush"),
      theme_manager->GetResourceBrush("text.caret.brush"));

  text_render_object_->SetAttachedControl(this);

  service_ = std::make_unique<TextHostControlService>(this);
  service_->SetEnabled(false);
  service_->SetEditable(false);
}

render::RenderObject* TextBlock::GetRenderObject() {
  return text_render_object_.get();
}

std::string TextBlock::GetText() { return service_->GetText(); }

void TextBlock::SetText(std::string text) {
  service_->SetText(std::move(text));
}

bool TextBlock::IsSelectable() { return service_->IsEnabled(); }

void TextBlock::SetSelectable(bool value) { service_->SetEnabled(value); }

void TextBlock::SetTextColor(const Color& color) {
  text_render_object_->SetBrush(platform::gui::IUiApplication::GetInstance()
                                    ->GetGraphicsFactory()
                                    ->CreateSolidColorBrush(color));
}

render::TextRenderObject* TextBlock::GetTextRenderObject() {
  return text_render_object_.get();
}
}  // namespace cru::ui::controls
