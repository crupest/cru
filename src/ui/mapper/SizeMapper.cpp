#include "cru/ui/mapper/SizeMapper.hpp"

namespace cru::ui::mapper {
bool SizeMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"Size") {
    return true;
  }
  return false;
}

Size SizeMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception(u"Invalid Point string.");
  }
}

Size SizeMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
