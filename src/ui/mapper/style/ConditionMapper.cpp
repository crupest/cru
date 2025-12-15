#include "cru/ui/mapper/style/ConditionMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/base/xml/XmlNode.h"
#include "cru/ui/mapper/MapperRegistry.h"

namespace cru::ui::mapper::style {
ClonePtr<NoCondition> NoConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  return NoCondition::Create();
}

ClonePtr<AndCondition> AndConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  std::vector<ClonePtr<Condition>> conditions;
  auto condition_mappers =
      MapperRegistry::GetInstance()->GetMappersByInterface<IConditionMapper>();
  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();
      for (auto mapper : condition_mappers) {
        if (mapper->XmlElementIsOfThisType(c)) {
          conditions.push_back(mapper->MapConditionFromXml(c));
          break;
        }
      }
    }
  }
  return AndCondition::Create(std::move(conditions));
}

ClonePtr<OrCondition> OrConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  std::vector<ClonePtr<Condition>> conditions;
  auto condition_mappers =
      MapperRegistry::GetInstance()->GetMappersByInterface<IConditionMapper>();
  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();
      for (auto mapper : condition_mappers) {
        if (mapper->XmlElementIsOfThisType(c)) {
          conditions.push_back(mapper->MapConditionFromXml(c));
          break;
        }
      }
    }
  }
  return OrCondition::Create(std::move(conditions));
}

ClonePtr<ClickStateCondition>
ClickStateConditionMapper::DoMapFromXml(xml::XmlElementNode* node) {
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

  return ClickStateCondition::Create(state);
}

ClonePtr<CheckedCondition> CheckedConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return CheckedCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return CheckedCondition::Create(false);
  } else {
    throw Exception("Invalid value for CheckedCondition: " + value);
  }
}

ClonePtr<FocusCondition> FocusConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return FocusCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return FocusCondition::Create(false);
  } else {
    throw Exception("Invalid value for FocusCondition: " + value);
  }
}

ClonePtr<HoverCondition> HoverConditionMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto value = node->GetAttributeValueCaseInsensitive("value");
  if (cru::string::CaseInsensitiveCompare(value, "true") == 0) {
    return HoverCondition::Create(true);
  } else if (cru::string::CaseInsensitiveCompare(value, "false") == 0) {
    return HoverCondition::Create(false);
  } else {
    throw Exception("Invalid value for HoverCondition: " + value);
  }
}
}  // namespace cru::ui::mapper::style
