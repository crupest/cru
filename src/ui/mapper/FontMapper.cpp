#include "cru/ui/mapper/FontMapper.hpp"
#include "../Helper.hpp"
#include "cru/platform/graphics/Factory.hpp"

namespace cru::ui::mapper {
bool FontMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"font");
}

std::shared_ptr<platform::graphics::IFont> FontMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto font_family_attr = node->GetOptionalAttribute(u"family");
  auto font_size_attr = node->GetOptionalAttribute(u"size");

  auto font_family = font_family_attr.value_or(u"");
  auto font_size = font_size_attr ? font_size_attr->ParseToFloat() : 24.0f;

  return GetGraphicsFactory()->CreateFont(font_family, font_size);
}
}  // namespace cru::ui::mapper
