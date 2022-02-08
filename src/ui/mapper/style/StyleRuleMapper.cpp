#include "cru/ui/mapper/style/StyleRuleMapper.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/style/IConditionMapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Condition.h"
#include "cru/ui/style/StyleRule.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
using namespace ui::style;
bool StyleRuleMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"StyleRule");
}

ClonablePtr<ui::style::StyleRule> StyleRuleMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  std::vector<IConditionMapper*> condition_mappers =
      MapperRegistry::GetInstance()->GetMappersByInterface<IConditionMapper>();
  std::vector<IStylerMapper*> styler_mappers =
      MapperRegistry::GetInstance()->GetMappersByInterface<IStylerMapper>();

  std::vector<ClonablePtr<Condition>> conditions;
  std::vector<ClonablePtr<Styler>> stylers;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();

      bool resolved = false;
      for (auto condition_mapper : condition_mappers) {
        if (condition_mapper->XmlElementIsOfThisType(c)) {
          conditions.push_back(condition_mapper->MapConditionFromXml(c));
          resolved = true;
          break;
        }
      }

      if (!resolved) {
        for (auto styler_mapper : styler_mappers) {
          if (styler_mapper->XmlElementIsOfThisType(c)) {
            stylers.push_back(styler_mapper->MapStylerFromXml(c));
            resolved = true;
            break;
          }
        }
      }
    }
  }

  return StyleRule::Create(AndCondition::Create(std::move(conditions)),
                           CompoundStyler::Create(std::move(stylers)));
}
}  // namespace cru::ui::mapper::style
