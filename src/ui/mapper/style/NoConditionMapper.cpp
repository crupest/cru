#include "cru/ui/mapper/style/NoConditionMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool NoConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "NoCondition") == 0;
}

ClonablePtr<ui::style::NoCondition> NoConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  return ui::style::NoCondition::Create();
}
}  // namespace cru::ui::mapper::style
