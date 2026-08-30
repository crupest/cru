#include "cru/ui/datamodel/FontDataType.h"

#include "cru/base/StringUtil.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::datamodel {
FontDataType::FontDataType()
    : SharedPtrDataTypeBase<platform::graphics::IFont>(
          "Font", {false, false, true, false}) {}

bool FontDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("Font");
}

DataConvertResult<std::shared_ptr<platform::graphics::IFont>>
FontDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto font_family_attr = node->GetOptionalAttributeValue("family");
  auto font_size_attr = node->GetOptionalAttributeValue("size");

  auto font_family = font_family_attr.value_or("");
  float font_size = 16.0f;

  if (font_size_attr) {
    auto parse_result = cru::string::ParseToNumber<float>(*font_size_attr);
    if (!parse_result.valid) {
      return DataConvertResult<std::shared_ptr<platform::graphics::IFont>>::
          Failure(std::format("Invalid font size: {}", parse_result.message));
    }
    font_size = parse_result.value;
  }

  return DataConvertResult<std::shared_ptr<platform::graphics::IFont>>::Success(
      platform::gui::IUiApplication::GetInstance()
          ->GetGraphicsFactory()
          ->CreateFont(std::string(font_family), font_size));
}
}  // namespace cru::ui::datamodel
