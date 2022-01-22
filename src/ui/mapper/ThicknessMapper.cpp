#include "cru/ui/mapper/ThicknessMapper.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
bool ThicknessMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"Thickness") {
    return true;
  }
  return false;
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
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
