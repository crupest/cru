#include "cru/ui/datamodel/CornerRadiusDataType.h"

namespace cru::ui::datamodel {
CornerRadiusDataType::CornerRadiusDataType()
    : DataTypeBase<CornerRadius>("CornerRadius",
                                   {false, false, true, false}) {}

bool CornerRadiusDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  return node->HasTag("CornerRadius");
}

DataConvertResult<CornerRadius> CornerRadiusDataType::DoConvertFromXml(
    xml::XmlElementNode* node) {
  auto* point_data_type = GetUiDataTypeRegistry()->GetDataType<Point>();
  if (!point_data_type) {
    return DataConvertResult<CornerRadius>::Failure(
        "PointDataType is not registered.");
  }

  CornerRadius result;

  auto apply_point_attribute = [point_data_type, &node](
                                   std::string_view key,
                                   auto&& apply) -> DataConvertResult<bool> {
    auto attr =
        node->GetOptionalAttributeValueCaseInsensitive(std::string(key));
    if (!attr) {
      return DataConvertResult<bool>::Success(false);
    }

    auto parsed = point_data_type->ConvertFromString(*attr);
    if (!parsed.IsSuccess()) {
      return DataConvertResult<bool>::Failure(parsed.GetErrors());
    }

    apply(parsed.GetValue());
    return DataConvertResult<bool>::SuccessWithErrors(
        true, std::vector<std::string>(parsed.GetErrors().begin(),
                                       parsed.GetErrors().end()));
  };

  std::vector<std::string> errors;

  auto all_result = apply_point_attribute(
      "all", [&result](const Point& point) { result.SetAll(point); });
  if (!all_result.IsSuccess()) {
    return DataConvertResult<CornerRadius>::Failure(all_result.GetErrors());
  }
  if (all_result.HasErrors()) {
    auto e = all_result.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto left_top_result = apply_point_attribute(
      "lefttop", [&result](const Point& point) { result.left_top = point; });
  if (!left_top_result.IsSuccess()) {
    return DataConvertResult<CornerRadius>::Failure(
        left_top_result.GetErrors());
  }
  if (left_top_result.HasErrors()) {
    auto e = left_top_result.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto right_top_result = apply_point_attribute(
      "righttop", [&result](const Point& point) { result.right_top = point; });
  if (!right_top_result.IsSuccess()) {
    return DataConvertResult<CornerRadius>::Failure(
        right_top_result.GetErrors());
  }
  if (right_top_result.HasErrors()) {
    auto e = right_top_result.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto right_bottom_result = apply_point_attribute(
      "rightbottom",
      [&result](const Point& point) { result.right_bottom = point; });
  if (!right_bottom_result.IsSuccess()) {
    return DataConvertResult<CornerRadius>::Failure(
        right_bottom_result.GetErrors());
  }
  if (right_bottom_result.HasErrors()) {
    auto e = right_bottom_result.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto left_bottom_result = apply_point_attribute(
      "leftbottom",
      [&result](const Point& point) { result.left_bottom = point; });
  if (!left_bottom_result.IsSuccess()) {
    return DataConvertResult<CornerRadius>::Failure(
        left_bottom_result.GetErrors());
  }
  if (left_bottom_result.HasErrors()) {
    auto e = left_bottom_result.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  if (errors.empty()) {
    return DataConvertResult<CornerRadius>::Success(result);
  }
  return DataConvertResult<CornerRadius>::SuccessWithErrors(result,
                                                            std::move(errors));
}
}  // namespace cru::ui::datamodel
