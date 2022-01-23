#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/mapper/style/FocusConditionMapper.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
bool FocusConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"FocusCondition");
}

ClonablePtr<ui::style::FocusCondition> FocusConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeCaseInsensitive(u"value");
  if (value.CaseInsensitiveEqual(u"true")) {
    return ui::style::FocusCondition::Create(true);
  } else if (value.CaseInsensitiveEqual(u"false")) {
    return ui::style::FocusCondition::Create(false);
  } else {
    throw Exception(u"Invalid value for FocusCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
