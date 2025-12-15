#include "cru/ui/mapper/StringMapper.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper {
std::string StringMapper::DoMapFromString(std::string str) {
  return std::move(str);
}

std::string StringMapper::DoMapFromXml(xml::XmlElementNode* node) {
  return MapFromXmlAsStringValue(node, "");
}
}  // namespace cru::ui::mapper
