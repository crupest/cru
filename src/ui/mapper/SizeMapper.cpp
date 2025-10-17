#include "cru/ui/mapper/SizeMapper.h"

#include <cru/base/String.h>

namespace cru::ui::mapper {
bool SizeMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "Size") == 0;
}

Size SizeMapper::DoMapFromString(std::string str) {
  std::vector<float> values = String::FromUtf8(str).ParseToFloatList();
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception("Invalid Point string.");
  }
}

Size SizeMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
