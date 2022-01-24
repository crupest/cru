#include "cru/ui/mapper/style/StyleRuleSetMapper.hpp"
#include <memory>
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/StyleRule.hpp"
#include "cru/ui/style/StyleRuleSet.hpp"

namespace cru::ui::mapper::style {
using namespace cru::ui::style;

bool StyleRuleSetMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"StyleRuleSet");
}

std::shared_ptr<ui::style::StyleRuleSet> StyleRuleSetMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto style_rule_mapper =
      MapperRegistry::GetInstance()->GetPtrMapper<StyleRule>();

  auto result = std::make_shared<StyleRuleSet>();

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();
      if (style_rule_mapper->XmlElementIsOfThisType(c)) {
        auto style_rule = style_rule_mapper->MapFromXml(c);
        result->AddStyleRule(*style_rule);
      } else {
        throw Exception(u"StyleRuleSet can only contain StyleRule.");
      }
    }
  }

  return result;
}
}  // namespace cru::ui::mapper::style
