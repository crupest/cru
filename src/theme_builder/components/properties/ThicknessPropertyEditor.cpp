#include "ThicknessPropertyEditor.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/ThicknessMapper.h"

namespace cru::theme_builder::components::properties {
ThicknessPropertyEditor::ThicknessPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetTextView();
    auto thickness_mapper =
        ui::mapper::MapperRegistry::GetInstance()->GetMapper<ui::Thickness>();
    try {
      auto thickness = thickness_mapper->MapFromString(text.ToString());
      thickness_ = thickness;
      is_text_valid_ = true;
      thickness_change_event_.Raise(thickness);
    } catch (const Exception &) {
      is_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

ThicknessPropertyEditor::~ThicknessPropertyEditor() {
  container_.RemoveFromParent();
}

void ThicknessPropertyEditor::SetThickness(const ui::Thickness &thickness) {
  thickness_ = thickness;
  text_.SetText(Format(u"{} {} {} {}", thickness_.left, thickness_.top,
                       thickness_.right, thickness_.bottom));
  is_text_valid_ = true;
  thickness_change_event_.Raise(thickness_);
}
}  // namespace cru::theme_builder::components::properties
