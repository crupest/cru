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
  font_size_container_.AddChild(&font_size_text_);
  font_size_label_.SetText(u"Font Size");

  font_family_text_.TextChangeEvent()->AddSpyOnlyHandler(
      [this] { RaiseChangeEvent(); });

  font_size_text_.TextChangeEvent()->AddSpyOnlyHandler(
      [this] { RaiseChangeEvent(); });
}

FontPropertyEditor::~FontPropertyEditor() {}

Control* FontPropertyEditor::GetRootControl() { return &main_container_; }

std::shared_ptr<platform::graphics::IFont> FontPropertyEditor::GetValue()
    const {
  return platform::gui::IUiApplication::GetInstance()
      ->GetGraphicsFactory()
      ->CreateFont(font_family_text_.GetText(),
                   font_size_text_.GetText().ParseToFloat());
}

}  // namespace cru::theme_builder::components::properties
