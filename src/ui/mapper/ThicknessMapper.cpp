#include "cru/ui/mapper/ThicknessMapper.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
bool ThicknessMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveCompare(u"Thickness") == 0;
}

Thickness ThicknessMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 4) {
    return Thickness(values[0], values[1], values[2], values[3]);
  } else if (values.size() == 2) {
    return Thickness(values[0], values[1], values[0], values[1]);
  } else if (values.size() == 1) {
    return Thickness(values[0], values[0], values[0], values[0]);
  } else {
    throw Exception(u"Invalid Thickness string.");
  }
}

Thickness ThicknessMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
