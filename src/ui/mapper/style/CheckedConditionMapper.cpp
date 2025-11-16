#include "cru/ui/mapper/style/CheckedConditionMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/style/Condition.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool CheckedConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(),
                                             "CheckedCondition") == 0;
}

ClonePtr<ui::style::CheckedCondition> CheckedConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return ui::style::CheckedCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return ui::style::CheckedCondition::Create(false);
  } else {
    throw Exception("Invalid value for CheckedCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
