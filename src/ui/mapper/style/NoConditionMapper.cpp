#include "cru/ui/mapper/style/NoConditionMapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
bool NoConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"NoCondition");
}

ClonablePtr<ui::style::NoCondition> NoConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  return ui::style::NoCondition::Create();
}
}  // namespace cru::ui::mapper::style
