#include "MeasureLengthPropertyEditor.h"
#include "cru/ui/datamodel/Base.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <string>

namespace cru::theme_builder::components::properties {
MeasureLengthPropertyEditor::MeasureLengthPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto* measure_length_data_type =
        ui::datamodel::GetUiDataTypeRegistry()
            ->GetDataType<ui::render::MeasureLength>();
    if (!measure_length_data_type) {
      is_text_valid_ = false;
      return;
    }

    auto measure_length_result = measure_length_data_type->ConvertFromString(text);
    if (measure_length_result.IsSuccess()) {
      measure_length_ = measure_length_result.GetValue();
      is_text_valid_ = true;
      RaiseChangeEvent();
    } else {
      is_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

MeasureLengthPropertyEditor::~MeasureLengthPropertyEditor() {}

void MeasureLengthPropertyEditor::SetValue(
    const ui::render::MeasureLength& value, bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  text_.SetText(measure_length_.IsSpecified()
                    ? std::to_string(measure_length_.GetLengthOrUndefined())
                    : "unspecified");
}
}  // namespace cru::theme_builder::components::properties
