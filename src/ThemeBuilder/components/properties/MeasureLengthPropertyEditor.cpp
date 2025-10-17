#include "MeasureLengthPropertyEditor.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <string>

namespace cru::theme_builder::components::properties {
MeasureLengthPropertyEditor::MeasureLengthPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto measure_length_mapper = ui::mapper::MapperRegistry::GetInstance()
                                     ->GetMapper<ui::render::MeasureLength>();
    try {
      auto measure_length =
          measure_length_mapper->MapFromString(text);
      measure_length_ = measure_length;
      is_text_valid_ = true;
      RaiseChangeEvent();
    } catch (const Exception&) {
      is_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

MeasureLengthPropertyEditor::~MeasureLengthPropertyEditor() {}

void MeasureLengthPropertyEditor::SetValue(
    const ui::render::MeasureLength& value, bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  text_.SetText(measure_length_.IsNotSpecified()
                    ? "unspecified"
                    : std::to_string(measure_length_.GetLengthOrUndefined()));
}
}  // namespace cru::theme_builder::components::properties
