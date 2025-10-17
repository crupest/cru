#include "ThicknessPropertyEditor.h"
#include <format>
#include "cru/base/Format.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/ThicknessMapper.h"

namespace cru::theme_builder::components::properties {
ThicknessPropertyEditor::ThicknessPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto thickness_mapper =
        ui::mapper::MapperRegistry::GetInstance()->GetMapper<ui::Thickness>();
    try {
      auto thickness = thickness_mapper->MapFromString(text);
      thickness_ = thickness;
      is_text_valid_ = true;
      RaiseChangeEvent();
    } catch (const Exception &) {
      is_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

ThicknessPropertyEditor::~ThicknessPropertyEditor() {}

void ThicknessPropertyEditor::SetValue(const ui::Thickness &thickness,
                                       bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  text_.SetText(std::format("{} {} {} {}", thickness.left, thickness.top,
                            thickness.right, thickness.bottom));
}
}  // namespace cru::theme_builder::components::properties
