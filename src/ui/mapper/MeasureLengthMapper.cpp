#include "cru/ui/mapper/MeasureLengthMapper.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/render/MeasureRequirement.h"

namespace cru::ui::mapper {
render::MeasureLength MeasureLengthMapper::DoMapFromString(std::string str) {
  if (cru::string::CaseInsensitiveCompare(str, "notspecified") == 0) {
    return render::MeasureLength::NotSpecified();
  }
  if (cru::string::CaseInsensitiveCompare(str, "unspecified") == 0) {
    return render::MeasureLength::NotSpecified();
  }
  auto value = cru::string::ParseToNumber<float>(str).value;
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
