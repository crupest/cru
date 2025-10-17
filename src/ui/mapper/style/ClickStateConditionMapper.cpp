#include "cru/ui/mapper/style/ClickStateConditionMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/base/Exception.h"
#include "cru/base/StringUtil.h"
#include "cru/ui/helper/ClickDetector.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
bool ClickStateConditionMapper::XmlElementIsOfThisType(
    xml::XmlElementNode *node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(),
                                             "ClickStateCondition") == 0;
}

ClonablePtr<ui::style::ClickStateCondition>
ClickStateConditionMapper::DoMapFromXml(xml::XmlElementNode *node) {
  auto state = helper::ClickState::None;
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attr) {
    if (cru::string::CaseInsensitiveCompare(*value_attr, "none") == 0) {
      state = helper::ClickState::None;
    } else if (cru::string::CaseInsensitiveCompare(*value_attr, "hover") == 0) {
      state = helper::ClickState::Hover;
    } else if (cru::string::CaseInsensitiveCompare(*value_attr, "press") == 0) {
      state = helper::ClickState::Press;
    } else if (cru::string::CaseInsensitiveCompare(*value_attr,
                                                   "pressinactive") == 0) {
      state = helper::ClickState::PressInactive;
    } else {
      throw Exception("Unknown click state: " + *value_attr);
    }
  }

  return ui::style::ClickStateCondition::Create(state);
}
}  // namespace cru::ui::mapper::style
