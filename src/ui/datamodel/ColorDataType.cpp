#include "cru/ui/datamodel/ColorDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
ColorDataType::ColorDataType()
    : DataTypeBase<Color>("Color", {true, false, true, false}) {}

bool ColorDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("Color");
}

DataConvertResult<Color> ColorDataType::DoConvertFromString(
    std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<Color>::Success(colors::transparent);
  }

  auto color = Color::Parse(value);
  if (!color) {
    return DataConvertResult<Color>::Failure("Invalid color string.");
  }
  return DataConvertResult<Color>::Success(*color);
}

DataConvertResult<Color> ColorDataType::DoConvertFromXml(
    xml::XmlElementNode* node) {
  auto converted = this->DoConvertFromXmlByString(node);
  if (!converted.IsSuccess()) {
    return converted;
  }

  auto value = converted.GetValue();

  auto alpha_value_attr =
      node->GetOptionalAttributeValueCaseInsensitive("alpha");
  if (alpha_value_attr) {
    auto alpha_result = cru::string::ParseToNumber<double>(*alpha_value_attr);
    // NOTE: Legacy mapper ignored ParseToNumber validity and used raw .value.
    if (!alpha_result.valid) {
      return DataConvertResult<Color>::Failure(
          "Invalid alpha value for Color.");
    }
    value.alpha = alpha_result.value * 255;
  }

  if (!converted.HasErrors()) {
    return DataConvertResult<Color>::Success(value);
  }

  return DataConvertResult<Color>::SuccessWithErrors(value,
                                                     converted.GetErrors());
}
}  // namespace cru::ui::datamodel
