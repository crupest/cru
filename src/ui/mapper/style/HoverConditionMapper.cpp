#include "cru/ui/mapper/style/HoverConditionMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
using namespace cru::ui::style;

bool HoverConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(),
                                             "HoverCondition") == 0;
}

ClonablePtr<HoverCondition> HoverConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return ui::style::HoverCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return ui::style::HoverCondition::Create(false);
  } else {
    throw Exception("Invalid value for HoverCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
