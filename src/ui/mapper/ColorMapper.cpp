#include "cru/ui/mapper/ColorMapper.h"

namespace cru::ui::mapper {
bool ColorMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "Color") == 0;
}

Color ColorMapper::DoMapFromString(std::string str) {
  auto c = Color::Parse(String::FromUtf8(str));
  if (!c) {
    throw Exception("Invalid color value.");
  }
  return *c;
}

Color ColorMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  Color result = colors::transparent;
  if (value_attr) {
    result = DoMapFromString(*value_attr);
  }

  auto alpha_value_attr =
      node->GetOptionalAttributeValueCaseInsensitive("alpha");
  if (alpha_value_attr) {
    result.alpha = String::FromUtf8(*alpha_value_attr).ParseToDouble() * 255;
  }

  return result;
}
}  // namespace cru::ui::mapper
