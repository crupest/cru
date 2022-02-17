#include "ColorPropertyEditor.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/ui/Base.h"

namespace cru::theme_builder::components::properties {
ColorPropertyEditor::ColorPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&color_cube_);
  container_.AddChild(&color_text_);

  color_cube_brush_ = platform::gui::IUiApplication::GetInstance()
                          ->GetGraphicsFactory()
                          ->CreateSolidColorBrush(color_);
  color_cube_.SetForegroundBrush(color_cube_brush_);
  color_text_.SetText(color_.ToString());

  color_text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = color_text_.GetTextView();
    auto color = ui::Color::Parse(text);
    if (color) {
      color_ = *color;
      color_cube_brush_->SetColor(*color);
      is_color_text_valid_ = true;
      RaiseChangeEvent();
    } else {
      is_color_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

ColorPropertyEditor::~ColorPropertyEditor() {}

void ColorPropertyEditor::SetValue(const ui::Color &color,
                                   bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  color_text_.SetText(color.ToString());
}
}  // namespace cru::theme_builder::components::properties
