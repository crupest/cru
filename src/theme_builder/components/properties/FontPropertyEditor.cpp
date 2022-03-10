#include "FontPropertyEditor.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/render/FlexLayoutRenderObject.h"

namespace cru::theme_builder::components::properties {
using namespace cru::ui::controls;

FontPropertyEditor::FontPropertyEditor() {
  main_container_.SetFlexDirection(FlexDirection::Horizontal);
  main_container_.AddChild(&label_);
  main_container_.AddChild(&right_container_);

  right_container_.SetFlexDirection(FlexDirection::Vertical);
  right_container_.AddChild(&font_family_container_);
  right_container_.AddChild(&font_size_container_);

  font_family_container_.SetFlexDirection(FlexDirection::Horizontal);
  font_family_container_.AddChild(&font_family_label_);
  font_family_container_.AddChild(&font_family_text_);
  font_family_label_.SetText(u"Font Family");

  font_size_container_.SetFlexDirection(FlexDirection::Horizontal);
  font_size_container_.AddChild(&font_size_label_);
  font_size_container_.AddChild(font_size_input_.GetRootControl());
  font_size_label_.SetText(u"Font Size");
  font_size_input_.SetMin(0.0f);

  font_family_text_.TextChangeEvent()->AddSpyOnlyHandler(
      [this] { RaiseChangeEvent(); });

  font_size_input_.ChangeEvent()->AddSpyOnlyHandler(
      [this] { RaiseChangeEvent(); });
}

FontPropertyEditor::~FontPropertyEditor() {}

Control* FontPropertyEditor::GetRootControl() { return &main_container_; }

std::shared_ptr<platform::graphics::IFont> FontPropertyEditor::GetValue()
    const {
  return platform::gui::IUiApplication::GetInstance()
      ->GetGraphicsFactory()
      ->CreateFont(font_family_text_.GetText(), font_size_input_.GetValue());
}

void FontPropertyEditor::SetValue(
    std::shared_ptr<platform::graphics::IFont> value, bool trigger_change) {
  SuppressNextChangeEvent();
  font_family_text_.SetText(value->GetFontName());
  SuppressNextChangeEvent();
  font_size_input_.SetValue(value->GetFontSize());

  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::properties
