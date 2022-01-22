#include "cru/ui/mapper/PointMapper.hpp"

namespace cru::ui::mapper {
bool PointMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"Point") {
    return true;
  }
  return false;
}

Point PointMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception(u"Invalid Point string.");
  }
}

Point PointMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
