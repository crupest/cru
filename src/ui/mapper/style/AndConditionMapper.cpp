#include "cru/ui/mapper/style/AndConditionMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/style/IConditionMapper.h"
#include "cru/ui/style/Condition.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
bool AndConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "AndCondition") == 0;
}

ClonePtr<ui::style::AndCondition> AndConditionMapper::DoMapFromXml(
    xml::XmlElementNode *node) {
  std::vector<ClonePtr<ui::style::Condition>> conditions;
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
  return ui::style::AndCondition::Create(std::move(conditions));
}
}  // namespace cru::ui::mapper::style
