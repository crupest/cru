#include "cru/ui/mapper/MeasureLengthMapper.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/render/MeasureRequirement.h"

namespace cru::ui::mapper {
bool MeasureLengthMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "MeasureLength") ==
         0;
}

render::MeasureLength MeasureLengthMapper::DoMapFromString(std::string str) {
  if (cru::string::CaseInsensitiveCompare(str, "notspecified") == 0) {
    return render::MeasureLength::NotSpecified();
  }
  if (cru::string::CaseInsensitiveCompare(str, "unspecified") == 0) {
    return render::MeasureLength::NotSpecified();
  }
  auto value = String::FromUtf8(str).ParseToFloat();
  if (value < 0) {
    return render::MeasureLength::NotSpecified();
  }
  return render::MeasureLength(value);
}

render::MeasureLength MeasureLengthMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
