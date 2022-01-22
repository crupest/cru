#include "cru/ui/mapper/ColorMapper.hpp"

namespace cru::ui::mapper {
bool ColorMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag() == u"Color";
}

Color ColorMapper::DoMapFromString(String str) {
  auto c = Color::Parse(str);
  if (!c) {
    throw Exception(u"Invalid color value.");
  }
  return *c;
}

Color ColorMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) {
    return colors::transparent;
  }
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
