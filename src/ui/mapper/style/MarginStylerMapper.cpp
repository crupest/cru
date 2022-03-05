#include "cru/ui/mapper/style/MarginStylerMapper.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
MarginStylerMapper::MarginStylerMapper() { SetAllowedTags({u"MarginStyler"}); }

MarginStylerMapper::~MarginStylerMapper() {}

ClonablePtr<ui::style::MarginStyler> MarginStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  Thickness thickness;

  auto thickness_mapper = MapperRegistry::GetInstance()->GetMapper<Thickness>();

  auto value_attribute = node->GetOptionalAttributeValueCaseInsensitive(u"value");
  if (value_attribute) {
    thickness = thickness_mapper->MapFromString(*value_attribute);
  }

  return ui::style::MarginStyler::Create(thickness);
}
}  // namespace cru::ui::mapper::style
