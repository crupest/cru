#include "cru/ui/datamodel/style/StylerDataType.h"

namespace cru::ui::datamodel::style {
namespace {
template <typename TDerived>
DataConvertResult<ClonePtr<Styler>> ConvertStylerResult(
    DataConvertResult<ClonePtr<TDerived>> result) {
  if (!result.IsSuccess()) {
    return DataConvertResult<ClonePtr<Styler>>::Failure(result.GetErrors());
  }

  ClonePtr<Styler> value = result.GetValue();
  if (!result.HasErrors()) {
    return DataConvertResult<ClonePtr<Styler>>::Success(std::move(value));
  }

  return DataConvertResult<ClonePtr<Styler>>::SuccessWithErrors(
      std::move(value), std::vector<std::string>(result.GetErrors().begin(),
                                                 result.GetErrors().end()));
}
}  // namespace

#define CRU_DEFINE_STYLER_DATA_TYPE_BASE(styler_name)              \
  styler_name##StylerDataType::styler_name##StylerDataType()       \
      : ClonePtrDataTypeBase<styler_name##Styler>(                   \
            #styler_name "Styler", {false, false, true, false}) {} \
                                                                   \
  bool styler_name##StylerDataType::DoXmlIsOfThisType(             \
      xml::XmlElementNode* node) {                                 \
    return node->HasTag(#styler_name "Styler");                    \
  }                                                                \
                                                                   \
  bool styler_name##StylerDataType::XmlElementIsOfThisType(        \
      xml::XmlElementNode* node) {                                 \
    return DoXmlIsOfThisType(node);                                \
  }                                                                \
                                                                   \
  DataConvertResult<ClonePtr<Styler>>                              \
  styler_name##StylerDataType::ConvertStylerFromXml(               \
      xml::XmlElementNode* node) {                                 \
    return ConvertStylerResult(ConvertFromXml(node));              \
  }

CRU_DEFINE_STYLER_DATA_TYPE_BASE(Border)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(ContentBrush)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(Cursor)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(Font)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(Margin)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(Padding)
CRU_DEFINE_STYLER_DATA_TYPE_BASE(PreferredSize)

#undef CRU_DEFINE_STYLER_DATA_TYPE_BASE

DataConvertResult<ClonePtr<BorderStyler>>
BorderStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* border_style_data_type =
      GetUiDataTypeRegistry()->GetDataType<ApplyBorderStyleInfo>();
  if (!border_style_data_type) {
    return DataConvertResult<ClonePtr<BorderStyler>>::Failure(
        "BorderStyleDataType is not registered.");
  }

  ApplyBorderStyleInfo border_style;
  std::vector<std::string> errors;

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlElementNode::Type::Element) {
      continue;
    }

    auto* child_element = child->AsElement();
    if (!border_style_data_type->XmlIsOfThisType(child_element)) {
      continue;
    }

    auto converted = border_style_data_type->ConvertFromXml(child_element);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<BorderStyler>>::Failure(
          converted.GetErrors());
    }

    border_style = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = BorderStyler::Create(std::move(border_style));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<BorderStyler>>::Success(std::move(value));
  }

  return DataConvertResult<ClonePtr<BorderStyler>>::SuccessWithErrors(
      std::move(value), std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::ContentBrushStyler>>
ContentBrushStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* brush_data_type =
      GetUiDataTypeRegistry()
          ->GetSharedPtrDataType<platform::graphics::IBrush>();
  if (!brush_data_type) {
    return DataConvertResult<ClonePtr<ui::style::ContentBrushStyler>>::Failure(
        "BrushDataType is not registered.");
  }

  std::shared_ptr<platform::graphics::IBrush> brush;
  std::vector<std::string> errors;

  for (auto* child_node : node->GetChildren()) {
    if (!child_node->IsElementNode()) {
      continue;
    }

    auto converted = brush_data_type->ConvertFromXml(child_node->AsElement());
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::ContentBrushStyler>>::
          Failure(converted.GetErrors());
    }

    brush = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::ContentBrushStyler::Create(std::move(brush));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::ContentBrushStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<ClonePtr<ui::style::ContentBrushStyler>>::
      SuccessWithErrors(std::move(value), std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::CursorStyler>>
CursorStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* cursor_data_type =
      GetUiDataTypeRegistry()->GetSharedPtrDataType<platform::gui::ICursor>();
  if (!cursor_data_type) {
    return DataConvertResult<ClonePtr<ui::style::CursorStyler>>::Failure(
        "CursorDataType is not registered.");
  }

  std::shared_ptr<platform::gui::ICursor> cursor;
  std::vector<std::string> errors;

  for (auto* child : node->GetChildren()) {
    if (child->GetType() != xml::XmlNode::Type::Element) {
      continue;
    }

    auto* child_element = child->AsElement();
    if (!cursor_data_type->XmlIsOfThisType(child_element)) {
      continue;
    }

    auto converted = cursor_data_type->ConvertFromXml(child_element);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::CursorStyler>>::Failure(
          converted.GetErrors());
    }

    cursor = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::CursorStyler::Create(std::move(cursor));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::CursorStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<
      ClonePtr<ui::style::CursorStyler>>::SuccessWithErrors(std::move(value),
                                                            std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::FontStyler>>
FontStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* font_data_type =
      GetUiDataTypeRegistry()
          ->GetSharedPtrDataType<platform::graphics::IFont>();
  if (!font_data_type) {
    return DataConvertResult<ClonePtr<ui::style::FontStyler>>::Failure(
        "FontDataType is not registered.");
  }

  std::shared_ptr<platform::graphics::IFont> font;
  std::vector<std::string> errors;

  for (auto* child_node : node->GetChildren()) {
    if (!child_node->IsElementNode()) {
      continue;
    }

    auto converted = font_data_type->ConvertFromXml(child_node->AsElement());
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::FontStyler>>::Failure(
          converted.GetErrors());
    }

    font = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::FontStyler::Create(std::move(font));
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::FontStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<ClonePtr<ui::style::FontStyler>>::SuccessWithErrors(
      std::move(value), std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::MarginStyler>>
MarginStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* thickness_data_type = GetUiDataTypeRegistry()->GetDataType<Thickness>();
  if (!thickness_data_type) {
    return DataConvertResult<ClonePtr<ui::style::MarginStyler>>::Failure(
        "ThicknessDataType is not registered.");
  }

  Thickness thickness;
  std::vector<std::string> errors;

  auto value_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attribute) {
    auto converted = thickness_data_type->ConvertFromString(*value_attribute);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::MarginStyler>>::Failure(
          converted.GetErrors());
    }
    thickness = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::MarginStyler::Create(thickness);
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::MarginStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<
      ClonePtr<ui::style::MarginStyler>>::SuccessWithErrors(std::move(value),
                                                            std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::PaddingStyler>>
PaddingStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* thickness_data_type = GetUiDataTypeRegistry()->GetDataType<Thickness>();
  if (!thickness_data_type) {
    return DataConvertResult<ClonePtr<ui::style::PaddingStyler>>::Failure(
        "ThicknessDataType is not registered.");
  }

  Thickness thickness;
  std::vector<std::string> errors;

  auto value_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attribute) {
    auto converted = thickness_data_type->ConvertFromString(*value_attribute);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::PaddingStyler>>::Failure(
          converted.GetErrors());
    }
    thickness = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::PaddingStyler::Create(thickness);
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::PaddingStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<
      ClonePtr<ui::style::PaddingStyler>>::SuccessWithErrors(std::move(value),
                                                             std::move(errors));
}

DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>
PreferredSizeStylerDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* measure_length_data_type =
      GetUiDataTypeRegistry()->GetDataType<render::MeasureLength>();
  if (!measure_length_data_type) {
    return DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>::Failure(
        "MeasureLengthDataType is not registered.");
  }

  render::MeasureSize size;
  std::vector<std::string> errors;

  auto width_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("width");
  if (width_attribute) {
    auto converted =
        measure_length_data_type->ConvertFromString(*width_attribute);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>::
          Failure(converted.GetErrors());
    }
    size.width = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto height_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("height");
  if (height_attribute) {
    auto converted =
        measure_length_data_type->ConvertFromString(*height_attribute);
    if (!converted.IsSuccess()) {
      return DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>::
          Failure(converted.GetErrors());
    }
    size.height = converted.GetValue();
    auto e = converted.GetErrors();
    errors.insert(errors.end(), e.begin(), e.end());
  }

  auto value = ui::style::PreferredSizeStyler::Create(size);
  if (errors.empty()) {
    return DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>::Success(
        std::move(value));
  }

  return DataConvertResult<ClonePtr<ui::style::PreferredSizeStyler>>::
      SuccessWithErrors(std::move(value), std::move(errors));
}
}  // namespace cru::ui::datamodel::style
