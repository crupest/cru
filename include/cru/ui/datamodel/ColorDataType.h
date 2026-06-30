#pragma once

#include "Base.h"

#include <cru/platform/Color.h>

namespace cru::ui::datamodel {
class CRU_UI_API ColorDataType : public DataTypeBase<Color> {
 public:
  ColorDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<Color> DoConvertFromString(std::string_view value) override;
  DataConvertResult<Color> DoConvertFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel
