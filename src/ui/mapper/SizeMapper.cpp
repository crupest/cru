#include "cru/ui/mapper/SizeMapper.hpp"

namespace cru::ui::mapper {
bool SizeMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  if (node->GetTag() == u"Size") {
    return true;
  }
  return false;
}

std::unique_ptr<Size> SizeMapper::DoMapFromString(String str) {
  std::vector<float> values = str.ParseToFloatList();
  if (values.size() == 2) {
    return std::make_unique<Size>(values[0], values[1]);
  } else if (values.size() == 1) {
    return std::make_unique<Size>(values[0], values[0]);
  } else {
    throw Exception(u"Invalid Point string.");
  }
}

std::unique_ptr<Size> SizeMapper::DoMapFromXml(xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return std::make_unique<Size>();
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
