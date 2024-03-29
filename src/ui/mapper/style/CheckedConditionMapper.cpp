#include "cru/common/ClonablePtr.h"
#include "cru/ui/mapper/style/CheckedConditionMapper.h"
#include "cru/ui/style/Condition.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool CheckedConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"CheckedCondition");
}

ClonablePtr<ui::style::CheckedCondition> CheckedConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive(u"value");
  if (value.CaseInsensitiveEqual(u"true")) {
    return ui::style::CheckedCondition::Create(true);
  } else if (value.CaseInsensitiveEqual(u"false")) {
    return ui::style::CheckedCondition::Create(false);
  } else {
    throw Exception(u"Invalid value for CheckedCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
