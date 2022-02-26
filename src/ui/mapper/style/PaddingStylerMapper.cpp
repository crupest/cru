#include "cru/ui/mapper/style/PaddingStylerMapper.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
PaddingStylerMapper::PaddingStylerMapper() {
  SetAllowedTags({u"PaddingStyler"});
}

PaddingStylerMapper::~PaddingStylerMapper() {}

ClonablePtr<ui::style::PaddingStyler> PaddingStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  Thickness thickness;

  auto thickness_mapper = MapperRegistry::GetInstance()->GetMapper<Thickness>();

  auto value_attribute = node->GetOptionalAttributeCaseInsensitive(u"value");
  if (value_attribute) {
    thickness = thickness_mapper->MapFromString(*value_attribute);
  }

  return ui::style::PaddingStyler::Create(thickness);
}
}  // namespace cru::ui::mapper::style
