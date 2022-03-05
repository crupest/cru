#include "cru/ui/mapper/StringMapper.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
StringMapper::StringMapper() { SetAllowedTags({u"String"}); }

StringMapper::~StringMapper() {}

String StringMapper::DoMapFromString(String str) { return std::move(str); }

String StringMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive(u"value");
  if (value_attr) return *value_attr;
  return {};
}
}  // namespace cru::ui::mapper
