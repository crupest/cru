#include "cru/ui/mapper/style/PreferredSizeStylerMapper.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
bool PreferredSizeStylerMapper::XmlElementIsOfThisType(
    xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "PreferredSizeStyler") == 0;
}

ClonablePtr<ui::style::PreferredSizeStyler>
PreferredSizeStylerMapper::DoMapFromXml(xml::XmlElementNode* node) {
  render::MeasureSize size;

  auto measure_length_mapper =
      MapperRegistry::GetInstance()->GetMapper<render::MeasureLength>();

  auto width_attribute = node->GetOptionalAttributeValueCaseInsensitive("width");
  if (width_attribute) {
    size.width = measure_length_mapper->MapFromString(*width_attribute);
  }

  auto height_attribute = node->GetOptionalAttributeValueCaseInsensitive("height");
  if (height_attribute) {
    size.height = measure_length_mapper->MapFromString(*height_attribute);
  }

  return ui::style::PreferredSizeStyler::Create(size);
}
}  // namespace cru::ui::mapper::style
