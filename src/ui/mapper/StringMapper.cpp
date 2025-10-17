#include "cru/ui/mapper/StringMapper.h"
#include "cru/xml/XmlNode.h"
#include "cru/base/String.h"

namespace cru::ui::mapper {
StringMapper::StringMapper() { SetAllowedTags({"String"}); }

StringMapper::~StringMapper() {}

String StringMapper::DoMapFromString(std::string str) {
  return String::FromUtf8(str);
}

String StringMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attr) return String::FromUtf8(*value_attr);
  return {};
}
}  // namespace cru::ui::mapper
