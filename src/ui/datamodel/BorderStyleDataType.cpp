#include "cru/ui/datamodel/BorderStyleDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
using cru::string::CaseInsensitiveCompare;

BorderStyleDataType::BorderStyleDataType()
    : DataTypeBase<ui::style::ApplyBorderStyleInfo>(
          "BorderStyle", {false, false, true, false}) {}

bool BorderStyleDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("BorderStyle");
}

DataConvertResult<ui::style::ApplyBorderStyleInfo>
BorderStyleDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* thickness_data_type = GetUiDataTypeRegistry()->GetDataType<Thickness>();
  auto* corner_radius_data_type =
      GetUiDataTypeRegistry()->GetDataType<CornerRadius>();
  auto* brush_data_type =
      GetUiDataTypeRegistry()
          ->GetSharedPtrDataType<platform::graphics::IBrush>();

  if (!thickness_data_type || !corner_radius_data_type || !brush_data_type) {
    return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
        "One or more required data types are not registered.");
  }

  ui::style::ApplyBorderStyleInfo result;
  std::vector<std::string> errors;

  for (auto* child : node->GetChildren()) {
    if (child->IsTextNode()) {
      return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
          "BorderStyle can't have text nodes.");
    }

    if (!child->IsElementNode()) {
      continue;
    }

    auto* element = child->AsElement();
    if (thickness_data_type->XmlIsOfThisType(element)) {
      auto converted = thickness_data_type->ConvertFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
            converted.GetErrors());
      }
      result.border_thickness = converted.GetValue();
      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
      continue;
    }

    if (corner_radius_data_type->XmlIsOfThisType(element)) {
      auto converted = corner_radius_data_type->ConvertFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
            converted.GetErrors());
      }
      result.border_radius = converted.GetValue();
      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
      continue;
    }

    if (brush_data_type->XmlIsOfThisType(element)) {
      auto converted = brush_data_type->ConvertFromXml(element);
      if (!converted.IsSuccess()) {
        return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
            converted.GetErrors());
      }

      auto brush = converted.GetValue();
      auto name = element->GetOptionalAttributeValueCaseInsensitive("name");
      if (name) {
        if (CaseInsensitiveCompare(*name, "foreground") == 0) {
          result.foreground_brush = std::move(brush);
        } else if (CaseInsensitiveCompare(*name, "background") == 0) {
          result.background_brush = std::move(brush);
        } else {
          return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Failure(
              "Invalid name of brush in BorderStyle.");
        }
      } else {
        // NOTE: Legacy mapper lets later unnamed brushes overwrite earlier
        // ones.
        result.border_brush = std::move(brush);
      }

      auto e = converted.GetErrors();
      errors.insert(errors.end(), e.begin(), e.end());
    }

    // NOTE: Legacy mapper silently ignored unknown child elements in
    // BorderStyle.
  }

  if (errors.empty()) {
    return DataConvertResult<ui::style::ApplyBorderStyleInfo>::Success(result);
  }

  return DataConvertResult<ui::style::ApplyBorderStyleInfo>::SuccessWithErrors(
      result, std::move(errors));
}
}  // namespace cru::ui::datamodel
