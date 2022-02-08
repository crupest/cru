#include "cru/ui/mapper/style/HoverConditionMapper.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
using namespace cru::ui::style;

bool HoverConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"HoverCondition");
}

ClonablePtr<HoverCondition> HoverConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeCaseInsensitive(u"value");
  if (value.CaseInsensitiveEqual(u"true")) {
    return ui::style::HoverCondition::Create(true);
  } else if (value.CaseInsensitiveEqual(u"false")) {
    return ui::style::HoverCondition::Create(false);
  } else {
    throw Exception(u"Invalid value for HoverCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
