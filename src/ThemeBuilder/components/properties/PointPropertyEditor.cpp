#include "PointPropertyEditor.h"
#include "cru/ui/datamodel/Base.h"

#include <format>

namespace cru::theme_builder::components::properties {
PointPropertyEditor::PointPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto* point_data_type =
        ui::datamodel::GetUiDataTypeRegistry()->GetDataType<ui::Point>();
    if (!point_data_type) {
      is_text_valid_ = false;
      return;
    }

    auto point_result = point_data_type->ConvertFromString(text);
    if (point_result.IsSuccess()) {
      point_ = point_result.GetValue();
      is_text_valid_ = true;
      RaiseChangeEvent();
    } else {
      is_text_valid_ = false;
      // TODO: Show error!
    }
  });
}

PointPropertyEditor::~PointPropertyEditor() {}

void PointPropertyEditor::SetValue(const ui::Point& point,
                                   bool trigger_change) {
  if (!trigger_change) SuppressNextChangeEvent();
  text_.SetText(ConvertPointToString(point));
}

std::string PointPropertyEditor::ConvertPointToString(const ui::Point& point) {
  return std::format("{} {}", point.x, point.y);
}
}  // namespace cru::theme_builder::components::properties
