#include "cru/ui/mapper/PointMapper.h"
#include "cru/base/StringUtil.h"

namespace cru::ui::mapper {
bool PointMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "Point") == 0;
}

Point PointMapper::DoMapFromString(std::string str) {
  std::vector<float> values = cru::string::ParseToNumberList<float>(str);
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception("Invalid Point string.");
  }
}

Point PointMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
