#include "cru/ui/mapper/ThicknessMapper.h"
#include "cru/base/Base.h"
#include "cru/base/StringUtil.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper {
Thickness ThicknessMapper::DoMapFromString(std::string str) {
  std::vector<float> values = cru::string::ParseToNumberList<float>(str);
  if (values.size() == 4) {
    return Thickness(values[0], values[1], values[2], values[3]);
  } else if (values.size() == 2) {
    return Thickness(values[0], values[1], values[0], values[1]);
  } else if (values.size() == 1) {
    return Thickness(values[0], values[0], values[0], values[0]);
  } else {
    throw Exception("Invalid Thickness string.");
  }
}

Thickness ThicknessMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value_attr) throw Exception("Thickness must have a 'value' attribute.");
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
