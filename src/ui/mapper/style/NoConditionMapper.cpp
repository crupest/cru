#include "cru/ui/mapper/style/NoConditionMapper.h"
#include "cru/common/ClonablePtr.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool NoConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"NoCondition");
}

ClonablePtr<ui::style::NoCondition> NoConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  return ui::style::NoCondition::Create();
}
}  // namespace cru::ui::mapper::style
