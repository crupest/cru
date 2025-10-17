#include "cru/ui/mapper/SizeMapper.h"

namespace cru::ui::mapper {
bool SizeMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveCompare(u"Size") == 0;
}

Size SizeMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception("Invalid Point string.");
  }
}

Size SizeMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
