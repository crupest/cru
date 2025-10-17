#include "cru/ui/mapper/style/StyleRuleSetMapper.h"
#include <memory>
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/StyleRule.h"
#include "cru/ui/style/StyleRuleSet.h"

namespace cru::ui::mapper::style {
using namespace cru::ui::style;

bool StyleRuleSetMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"StyleRuleSet");
}

std::shared_ptr<ui::style::StyleRuleSet> StyleRuleSetMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto style_rule_mapper =
      MapperRegistry::GetInstance()->GetClonablePtrMapper<StyleRule>();

  auto result = std::make_shared<StyleRuleSet>();

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();
      if (style_rule_mapper->XmlElementIsOfThisType(c)) {
        auto style_rule = style_rule_mapper->MapFromXml(c);
        result->AddStyleRule(*style_rule);
      } else {
        throw Exception("StyleRuleSet can only contain StyleRule.");
      }
    }
  }

  return result;
}
}  // namespace cru::ui::mapper::style
