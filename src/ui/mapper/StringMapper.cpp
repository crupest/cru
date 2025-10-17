#include "cru/ui/mapper/StringMapper.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
StringMapper::StringMapper() { SetAllowedTags({"String"}); }

StringMapper::~StringMapper() {}

std::string StringMapper::DoMapFromString(std::string str) {
  return std::move(str);
}

std::string StringMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attr) return *value_attr;
  return {};
}
}  // namespace cru::ui::mapper
