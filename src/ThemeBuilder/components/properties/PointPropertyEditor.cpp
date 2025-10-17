#include "PointPropertyEditor.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/PointMapper.h"

#include <format>

namespace cru::theme_builder::components::properties {
PointPropertyEditor::PointPropertyEditor() {
  container_.AddChild(&label_);
  container_.AddChild(&text_);

  text_.TextChangeEvent()->AddHandler([this](std::nullptr_t) {
    auto text = text_.GetText();
    auto point_mapper =
        ui::mapper::MapperRegistry::GetInstance()->GetMapper<ui::Point>();
    try {
      auto point = point_mapper->MapFromString(text);
      point_ = point;
      is_text_valid_ = true;
      RaiseChangeEvent();
    } catch (const Exception&) {
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
