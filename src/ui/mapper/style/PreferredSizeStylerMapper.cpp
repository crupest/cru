#include "cru/ui/mapper/style/PreferredSizeStylerMapper.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
bool PreferredSizeStylerMapper::XmlElementIsOfThisType(
    xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"PreferredSizeStyler");
}

ClonablePtr<ui::style::PreferredSizeStyler>
PreferredSizeStylerMapper::DoMapFromXml(xml::XmlElementNode* node) {
  render::MeasureSize size;
  auto width_attribute = node->GetOptionalAttributeCaseInsensitive(u"width");
  if (width_attribute) {
    size.width = width_attribute->ParseToFloat();
  }

  auto height_attribute = node->GetOptionalAttributeCaseInsensitive(u"height");
  if (height_attribute) {
    size.height = height_attribute->ParseToFloat();
  }

  return ui::style::PreferredSizeStyler::Create(size);
}
}  // namespace cru::ui::mapper::style
