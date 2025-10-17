#include "cru/ui/mapper/style/FocusConditionMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/style/Condition.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool FocusConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(),
                                             "FocusCondition") == 0;
}

ClonablePtr<ui::style::FocusCondition> FocusConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return ui::style::FocusCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return ui::style::FocusCondition::Create(false);
  } else {
    throw Exception("Invalid value for FocusCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
