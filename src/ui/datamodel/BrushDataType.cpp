#include "cru/ui/datamodel/BrushDataType.h"

#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::datamodel {
BrushDataType::BrushDataType()
    : SharedPtrDataTypeBase<platform::graphics::IBrush>(
          "Brush", {false, false, true, false}) {}

bool BrushDataType::DoXmlIsOfThisType(xml::XmlElementNode* node) {
  auto* color_data_type = GetUiDataTypeRegistry()->GetDataType<Color>();
  return (color_data_type != nullptr &&
          color_data_type->XmlIsOfThisType(node)) ||
         node->HasTag("Brush");
}

DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>
BrushDataType::DoConvertFromXml(xml::XmlElementNode* node) {
  auto* color_data_type = GetUiDataTypeRegistry()->GetDataType<Color>();
  if (!color_data_type) {
    return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
        Failure("ColorDataType is not registered.");
  }

  auto* graphics_factory =
      platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory();

  if (color_data_type->XmlIsOfThisType(node)) {
    auto color_result = color_data_type->ConvertFromXml(node);
    if (!color_result.IsSuccess()) {
      return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
          Failure(color_result.GetErrors());
    }

    auto brush =
        graphics_factory->CreateSolidColorBrush(color_result.GetValue());
    if (!color_result.HasErrors()) {
      return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
          Success(std::move(brush));
    }

    return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
        SuccessWithErrors(
            std::move(brush),
            std::vector<std::string>(color_result.GetErrors().begin(),
                                     color_result.GetErrors().end()));
  }

  for (auto* child : node->GetChildren()) {
    if (auto* child_element = child->AsElement()) {
      // NOTE: Legacy mapper checked the parent node type here, not child type.
      // This refactor uses child type checking because parent-based checking is
      // likely an implementation bug, but behavior intent is not documented.
      if (!color_data_type->XmlIsOfThisType(child_element)) {
        return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
            Failure("Invalid child element of Brush.");
      }

      auto color_result = color_data_type->ConvertFromXml(child_element);
      if (!color_result.IsSuccess()) {
        return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
            Failure(color_result.GetErrors());
      }

      auto brush =
          graphics_factory->CreateSolidColorBrush(color_result.GetValue());
      if (!color_result.HasErrors()) {
        return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
            Success(std::move(brush));
      }

      return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
          SuccessWithErrors(
              std::move(brush),
              std::vector<std::string>(color_result.GetErrors().begin(),
                                       color_result.GetErrors().end()));
    }

    if (child->IsTextNode()) {
      return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
          Failure("Text node is not allowed in Brush.");
    }
  }

  return DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>::
      Failure("Brush doesn't have content.");
}
}  // namespace cru::ui::datamodel
