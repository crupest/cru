#include "cru/ui/datamodel/style/StyleRuleSetDataType.h"

namespace cru::ui::datamodel::style {
StyleRuleSetDataType::StyleRuleSetDataType()
    : SharedPtrDataTypeBase<ui::style::StyleRuleSet>(
          "StyleRuleSet", {false, false, true, false}) {}

bool StyleRuleSetDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("StyleRuleSet");
}

DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>
StyleRuleSetDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* style_rule_data_type =
      GetUiDataTypeRegistry()->GetSharedPtrDataType<ui::style::StyleRule>();
  if (!style_rule_data_type) {
    return DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>::Failure(
        "StyleRuleDataType is not registered.");
  }

  auto result = std::make_shared<ui::style::StyleRuleSet>();
  std::vector<std::string> errors;

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlNode::Type::Element) {
      continue;
    }

    auto* element = child->AsElement();
    if (!style_rule_data_type->XmlIsOfThisType(element)) {
      return DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>::
          Failure("StyleRuleSet can only contain StyleRule.");
    }

    auto converted = style_rule_data_type->ConvertFromXml(element);
    if (!converted.IsSuccess()) {
      return DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>::
          Failure(converted.GetErrors());
    }

    auto style_rule = converted.GetValue();
    result->AddStyleRule(*style_rule);

    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  if (errors.empty()) {
    return DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>::Success(
        std::move(result));
  }

  return DataConvertResult<std::shared_ptr<ui::style::StyleRuleSet>>::
      SuccessWithErrors(std::move(result), std::move(errors));
}
}  // namespace cru::ui::datamodel::style
