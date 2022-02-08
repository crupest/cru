#include "cru/ui/mapper/ColorMapper.h"

namespace cru::ui::mapper {
bool ColorMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveCompare(u"Color") == 0;
}

Color ColorMapper::DoMapFromString(String str) {
  auto c = Color::Parse(str);
  if (!c) {
    throw Exception(u"Invalid color value.");
  }
  return *c;
}

Color ColorMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeCaseInsensitive(u"value");
  Color result = colors::transparent;
  if (value_attr) {
    result = DoMapFromString(*value_attr);
  }

  auto alpha_value_attr = node->GetOptionalAttributeCaseInsensitive(u"alpha");
  if (alpha_value_attr) {
    result.alpha = alpha_value_attr->ParseToDouble() * 255;
  }

  return result;
}
}  // namespace cru::ui::mapper
