#include "cru/ui/mapper/style/ClickStateConditionMapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/common/Exception.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/style/Condition.hpp"

namespace cru::ui::mapper::style {
bool ClickStateConditionMapper::XmlElementIsOfThisType(
    xml::XmlElementNode *node) {
  return node->GetTag().CaseInsensitiveEqual(u"ClickStateCondition");
}

ClonablePtr<ui::style::ClickStateCondition>
ClickStateConditionMapper::DoMapFromXml(xml::XmlElementNode *node) {
  auto state = helper::ClickState::None;
  auto value_attr = node->GetOptionalAttributeCaseInsensitive(u"value");
  if (value_attr) {
    if (value_attr->CaseInsensitiveEqual(u"none")) {
      state = helper::ClickState::None;
    } else if (value_attr->CaseInsensitiveEqual(u"hover")) {
      state = helper::ClickState::Hover;
    } else if (value_attr->CaseInsensitiveEqual(u"press")) {
      state = helper::ClickState::Press;
    } else if (value_attr->CaseInsensitiveEqual(u"pressinactive")) {
      state = helper::ClickState::PressInactive;
    } else {
      throw Exception(u"Unknown click state: " + *value_attr);
    }
  }

  return ui::style::ClickStateCondition::Create(state);
}
}  // namespace cru::ui::mapper::style
