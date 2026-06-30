#include "cru/ui/datamodel/style/ConditionDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel::style {
namespace {
template <typename TDerived>
DataConvertResult<ClonePtr<Condition>> ConvertConditionResult(
    DataConvertResult<ClonePtr<TDerived>> result) {
  if (!result.IsSuccess()) {
    return DataConvertResult<ClonePtr<Condition>>::Failure(result.GetErrors());
  }

  ClonePtr<Condition> value = result.GetValue();
  if (!result.HasErrors()) {
    return DataConvertResult<ClonePtr<Condition>>::Success(std::move(value));
  }

  return DataConvertResult<ClonePtr<Condition>>::SuccessWithErrors(
      std::move(value), std::vector<std::string>(result.GetErrors().begin(),
                                                 result.GetErrors().end()));
}
}  // namespace

#define CRU_DEFINE_CONDITION_DATA_TYPE_BASE(condition_name)              \
  condition_name##ConditionDataType::condition_name##ConditionDataType() \
      : ClonePtrDataTypeBase<condition_name##Condition>(                   \
            #condition_name "Condition", {false, false, true, false}) {} \
                                                                         \
  bool condition_name##ConditionDataType::DoXmlIsOfThisType(             \
      xml::XmlElementNode* node) {                                       \
    return node->HasTag(#condition_name "Condition");                    \
  }                                                                      \
                                                                         \
  bool condition_name##ConditionDataType::XmlElementIsOfThisType(        \
      xml::XmlElementNode* node) {                                       \
    return DoXmlIsOfThisType(node);                                      \
  }                                                                      \
                                                                         \
  DataConvertResult<ClonePtr<Condition>>                                 \
  condition_name##ConditionDataType::ConvertConditionFromXml(            \
      xml::XmlElementNode* node) {                                       \
    return ConvertConditionResult(ConvertFromXml(node));                 \
  }

CRU_DEFINE_CONDITION_DATA_TYPE_BASE(No)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(And)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(Or)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(ClickState)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(Checked)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(Focus)
CRU_DEFINE_CONDITION_DATA_TYPE_BASE(Hover)

#undef CRU_DEFINE_CONDITION_DATA_TYPE_BASE

DataConvertResult<ClonePtr<NoCondition>> NoConditionDataType::DoConvertFromXml(
    xml::XmlElementNode* node) {
  CRU_UNUSED(node)
  return DataConvertResult<ClonePtr<NoCondition>>::Success(
      NoCondition::Create());
}

DataConvertResult<ClonePtr<AndCondition>>
AndConditionDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  std::vector<ClonePtr<Condition>> conditions;
  std::vector<std::string> errors;

  auto condition_data_types =
      GetUiDataTypeRegistry()->GetDataTypesByInterface<IConditionDataType>();

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlNode::Type::Element) {
      continue;
    }

    auto* element = child->AsElement();
    for (auto* data_type : condition_data_types) {
      if (!data_type->XmlElementIsOfThisType(element)) {
        continue;
      }

      auto converted = data_type->ConvertConditionFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<ClonePtr<AndCondition>>::Failure(
            converted.GetErrors());
      }

      conditions.push_back(converted.GetValue());
      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
      break;
    }
  }

  auto value = AndCondition::Create(std::move(conditions));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<AndCondition>>::Success(std::move(value));
  }

  return DataConvertResult<ClonePtr<AndCondition>>::SuccessWithErrors(
      std::move(value), std::move(errors));
}

DataConvertResult<ClonePtr<OrCondition>> OrConditionDataType::DoConvertFromXml(
    xml::XmlElementNode* node) {
  std::vector<ClonePtr<Condition>> conditions;
  std::vector<std::string> errors;

  auto condition_data_types =
      GetUiDataTypeRegistry()->GetDataTypesByInterface<IConditionDataType>();

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlNode::Type::Element) {
      continue;
    }

    auto* element = child->AsElement();
    for (auto* data_type : condition_data_types) {
      if (!data_type->XmlElementIsOfThisType(element)) {
        continue;
      }

      auto converted = data_type->ConvertConditionFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<ClonePtr<OrCondition>>::Failure(
            converted.GetErrors());
      }

      conditions.push_back(converted.GetValue());
      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
      break;
    }
  }

  auto value = OrCondition::Create(std::move(conditions));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<OrCondition>>::Success(std::move(value));
  }

  return DataConvertResult<ClonePtr<OrCondition>>::SuccessWithErrors(
      std::move(value), std::move(errors));
}

DataConvertResult<ClonePtr<ClickStateCondition>>
ClickStateConditionDataType::DoConvertFromXml(xml::XmlElementNode* node) {
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
      return DataConvertResult<ClonePtr<ClickStateCondition>>::Failure(
          "Unknown click state: " + *value_attr);
    }
  }

  return DataConvertResult<ClonePtr<ClickStateCondition>>::Success(
      ClickStateCondition::Create(state));
}

DataConvertResult<ClonePtr<CheckedCondition>>
CheckedConditionDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto value = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value) {
    return DataConvertResult<ClonePtr<CheckedCondition>>::Failure(
        "Missing value for CheckedCondition.");
  }

  if (cru::string::CaseInsensitiveCompare(*value, "true") == 0) {
    return DataConvertResult<ClonePtr<CheckedCondition>>::Success(
        CheckedCondition::Create(true));
  }
  if (cru::string::CaseInsensitiveCompare(*value, "false") == 0) {
    return DataConvertResult<ClonePtr<CheckedCondition>>::Success(
        CheckedCondition::Create(false));
  }

  return DataConvertResult<ClonePtr<CheckedCondition>>::Failure(
      "Invalid value for CheckedCondition: " + *value);
}

DataConvertResult<ClonePtr<FocusCondition>>
FocusConditionDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto value = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value) {
    return DataConvertResult<ClonePtr<FocusCondition>>::Failure(
        "Missing value for FocusCondition.");
  }

  if (cru::string::CaseInsensitiveCompare(*value, "true") == 0) {
    return DataConvertResult<ClonePtr<FocusCondition>>::Success(
        FocusCondition::Create(true));
  }
  if (cru::string::CaseInsensitiveCompare(*value, "false") == 0) {
    return DataConvertResult<ClonePtr<FocusCondition>>::Success(
        FocusCondition::Create(false));
  }

  return DataConvertResult<ClonePtr<FocusCondition>>::Failure(
      "Invalid value for FocusCondition: " + *value);
}

DataConvertResult<ClonePtr<HoverCondition>>
HoverConditionDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto value = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value) {
    return DataConvertResult<ClonePtr<HoverCondition>>::Failure(
        "Missing value for HoverCondition.");
  }

  if (cru::string::CaseInsensitiveCompare(*value, "true") == 0) {
    return DataConvertResult<ClonePtr<HoverCondition>>::Success(
        HoverCondition::Create(true));
  }
  if (cru::string::CaseInsensitiveCompare(*value, "false") == 0) {
    return DataConvertResult<ClonePtr<HoverCondition>>::Success(
        HoverCondition::Create(false));
  }

  return DataConvertResult<ClonePtr<HoverCondition>>::Failure(
      "Invalid value for HoverCondition: " + *value);
}
}  // namespace cru::ui::datamodel::style
