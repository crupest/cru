#include "cru/ui/mapper/SizeMapper.h"
#include "cru/base/StringUtil.h"

namespace cru::ui::mapper {
Size SizeMapper::DoMapFromString(std::string str) {
  std::vector<float> values = cru::string::ParseToNumberList<float>(str);
  if (values.size() == 2) {
    return {values[0], values[1]};
  } else if (values.size() == 1) {
    return {values[0], values[0]};
  } else {
    throw Exception("Invalid Size string.");
  }
}

Size SizeMapper::DoMapFromXml(xml::XmlElementNode* node) {
  return MapFromXmlAsStringValue(node, Size{});
}
}  // namespace cru::ui::mapper
