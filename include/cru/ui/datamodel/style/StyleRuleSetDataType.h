#pragma once

#include "../../style/StyleRuleSet.h"
#include "../Base.h"

namespace cru::ui::datamodel::style {
class CRU_UI_API StyleRuleSetDataType
    : public SharedPtrDataTypeBase<ui::style::StyleRuleSet> {
 public:
  StyleRuleSetDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>> DoConvertFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel::style
