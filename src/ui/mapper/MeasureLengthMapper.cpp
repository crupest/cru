#include "cru/ui/mapper/MeasureLengthMapper.h"
#include "cru/ui/render/MeasureRequirement.h"

namespace cru::ui::mapper {
bool MeasureLengthMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"MeasureLength");
}

render::MeasureLength MeasureLengthMapper::DoMapFromString(String str) {
  if (str.CaseInsensitiveEqual(u"notspecified")) {
    return render::MeasureLength::NotSpecified();
  }
  if (str.CaseInsensitiveEqual(u"unspecified")) {
    return render::MeasureLength::NotSpecified();
  }
  auto value = str.ParseToFloat();
  if (value < 0) {
    return render::MeasureLength::NotSpecified();
  }
  return render::MeasureLength(value);
}

render::MeasureLength MeasureLengthMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value_attr = node->GetOptionalAttributeCaseInsensitive(u"value");
  if (!value_attr) return {};
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
