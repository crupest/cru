#include "cru/ui/mapper/ThicknessMapper.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
bool ThicknessMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"Thickness") {
    return true;
  }
  return false;
}

std::unique_ptr<Thickness> ThicknessMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 4) {
    return std::make_unique<Thickness>(values[0], values[1], values[2],
                                       values[3]);
  } else if (values.size() == 2) {
    return std::make_unique<Thickness>(values[0], values[1], values[0],
                                       values[1]);
  } else if (values.size() == 1) {
    return std::make_unique<Thickness>(values[0], values[0], values[0],
                                       values[0]);
  } else {
    throw Exception(u"Invalid Thickness string.");
  }
}

std::unique_ptr<Thickness> ThicknessMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return std::make_unique<Thickness>();
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
