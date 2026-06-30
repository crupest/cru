#include "ThicknessPropertyEditor.h"
#include <format>
#include "cru/ui/datamodel/Base.h"

namespace cru::theme_builder::components::properties {
ThicknessPropertyEditor::ThicknessPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto* thickness_data_type =
        ui::datamodel::GetUiDataTypeRegistry()->GetDataType<ui::Thickness>();
    if (!thickness_data_type) {
      is_text_valid_ = false;
      return;
    }

    auto thickness_result = thickness_data_type->ConvertFromString(text);
    if (thickness_result.IsSuccess()) {
      thickness_ = thickness_result.GetValue();
      is_text_valid_ = true;
      RaiseChangeEvent();
    } else {
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
