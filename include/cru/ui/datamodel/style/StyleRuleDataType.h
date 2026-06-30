#pragma once

#include "../../style/StyleRule.h"
#include "../Base.h"

namespace cru::ui::datamodel::style {
class CRU_UI_API StyleRuleDataType
    : public SharedPtrDataTypeBase<ui::style::StyleRule> {
 public:
  StyleRuleDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<std::shared_ptr<ui::style::StyleRule>> DoConvertFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel::style
