#pragma once

#include "Base.h"

#include <cru/platform/graphics/Font.h>

namespace cru::ui::datamodel {
/**
 * Example xml:
 * ```xml
 * <Font family="Arial" size="16"/>
 * ```
 */
class CRU_UI_API FontDataType
    : public SharedPtrDataTypeBase<platform::graphics::IFont> {
 public:
  FontDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<std::shared_ptr<platform::graphics::IFont>>
  DoConvertFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel
