#include "cru/base/ClonablePtr.h"
#include "cru/ui/mapper/style/FocusConditionMapper.h"
#include "cru/ui/style/Condition.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool FocusConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"FocusCondition");
}

ClonablePtr<ui::style::FocusCondition> FocusConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive(u"value");
  if (value.CaseInsensitiveEqual(u"true")) {
    return ui::style::FocusCondition::Create(true);
  } else if (value.CaseInsensitiveEqual(u"false")) {
    return ui::style::FocusCondition::Create(false);
  } else {
    throw Exception(u"Invalid value for FocusCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
