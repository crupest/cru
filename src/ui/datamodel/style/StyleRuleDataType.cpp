#include "cru/ui/datamodel/style/StyleRuleDataType.h"

#include "cru/ui/datamodel/style/ConditionDataType.h"
#include "cru/ui/datamodel/style/StylerDataType.h"

namespace cru::ui::datamodel::style {
using namespace ui::style;

StyleRuleDataType::StyleRuleDataType()
    : SharedPtrDataTypeBase<ui::style::StyleRule>("StyleRule",
                                                {false, false, true, false}) {}

bool StyleRuleDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("StyleRule");
}

DataConvertResult<std::shared_ptr<ui::style::StyleRule>>
StyleRuleDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto condition_data_types =
      GetUiDataTypeRegistry()->GetDataTypesByInterface<IConditionDataType>();
  auto styler_data_types =
      GetUiDataTypeRegistry()->GetDataTypesByInterface<IStylerDataType>();

  std::vector<ClonePtr<Condition>> conditions;
  std::vector<ClonePtr<Styler>> stylers;
  std::vector<std::string> errors;

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlNode::Type::Element) {
      continue;
    }

    auto* element = child->AsElement();

    bool resolved = false;
    for (auto* condition_data_type : condition_data_types) {
      if (!condition_data_type->XmlElementIsOfThisType(element)) {
        continue;
      }

      auto converted = condition_data_type->ConvertConditionFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<std::shared_ptr<ui::style::StyleRule>>::
            Failure(converted.GetErrors());
      }

      conditions.push_back(converted.GetValue());
      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
      resolved = true;
      break;
    }

    if (!resolved) {
      for (auto* styler_data_type : styler_data_types) {
        if (!styler_data_type->XmlElementIsOfThisType(element)) {
          continue;
        }

        auto converted = styler_data_type->ConvertStylerFromXml(element);
        if (!converted.IsSuccess()) {
          return DataConvertResult<std::shared_ptr<ui::style::StyleRule>>::
              Failure(converted.GetErrors());
        }

        stylers.push_back(converted.GetValue());
        auto e = converted.GetErrors();
        errors.insert(errors.end(), e.begin(), e.end());
        resolved = true;
        break;
      }
    }

    if (!resolved) {
      return DataConvertResult<std::shared_ptr<ui::style::StyleRule>>::Failure(
          "Unknown element in StyleRule: " + element->GetTag());
    }
  }

  auto value =
      std::make_shared<StyleRule>(AndCondition::Create(std::move(conditions)),
                                  CompoundStyler::Create(std::move(stylers)));

  if (errors.empty()) {
    return DataConvertResult<std::shared_ptr<ui::style::StyleRule>>::Success(
        std::move(value));
  }

  return DataConvertResult<std::shared_ptr<ui::style::StyleRule>>::
      SuccessWithErrors(std::move(value), std::move(errors));
}
}  // namespace cru::ui::datamodel::style
