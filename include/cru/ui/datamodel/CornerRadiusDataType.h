#pragma once

#include "Base.h"

namespace cru::ui::datamodel {
class CRU_UI_API CornerRadiusDataType : public DataTypeBase<CornerRadius> {
 public:
  CornerRadiusDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<CornerRadius> DoConvertFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel
