#include "cru/ui/mapper/style/OrConditionMapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/mapper/style/IConditionMapper.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
bool OrConditionMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  return node->GetTag().CaseInsensitiveEqual(u"OrCondition");
}

ClonablePtr<ui::style::OrCondition> OrConditionMapper::DoMapFromXml(
    xml::XmlElementNode *node) {
  std::vector<ClonablePtr<ui::style::Condition>> conditions;
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
  return ui::style::OrCondition::Create(std::move(conditions));
}
}  // namespace cru::ui::mapper::style
