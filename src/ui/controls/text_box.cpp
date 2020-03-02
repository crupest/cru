#include "cru/ui/controls/text_box.hpp"

#include "cru/ui/render/border_render_object.hpp"
#include "cru/ui/render/canvas_render_object.hpp"
#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_manager.hpp"

namespace cru::ui::controls {
using render::BorderRenderObject;
using render::CanvasRenderObject;
using render::TextRenderObject;

TextBox::TextBox()
    : border_render_object_(new BorderRenderObject()),
      text_render_object_(),
      caret_render_object_(new CanvasRenderObject()) {
  const auto theme_resources = UiManager::GetInstance()->GetThemeResources();

  text_render_object_ = std::make_unique<TextRenderObject>(
      theme_resources->text_brush, theme_resources->default_font,
      theme_resources->text_selection_brush);

  border_render_object_->SetAttachedControl(this);
  text_render_object_->SetAttachedControl(this);
  caret_render_object_->SetAttachedControl(this);
}

TextBox::~TextBox() {}
}  // namespace cru::ui::controls
