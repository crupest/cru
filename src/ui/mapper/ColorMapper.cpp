#include "cru/ui/mapper/ColorMapper.h"
#include "cru/base/StringUtil.h"

namespace cru::ui::mapper {
Color ColorMapper::DoMapFromString(std::string str) {
  auto c = Color::Parse(str);
  if (!c) {
    throw Exception("Invalid color string.");
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
    result.alpha =
        cru::string::ParseToNumber<double>(*alpha_value_attr).value * 255;
  }

  return result;
}
}  // namespace cru::ui::mapper
