#include "cru/ui/mapper/style/StylerMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/base/xml/XmlNode.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
ClonePtr<BorderStyler> BorderStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto border_style_mapper =
      MapperRegistry::GetInstance()->GetMapper<ApplyBorderStyleInfo>();

  ApplyBorderStyleInfo border_style;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlElementNode::Type::Element) {
      auto child_element = child->AsElement();
      if (border_style_mapper->XmlElementIsOfThisType(child_element)) {
        border_style = border_style_mapper->MapFromXml(child_element);
      }
    }
  }

  return BorderStyler::Create(std::move(border_style));
}

ClonePtr<ui::style::ContentBrushStyler> ContentBrushStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto brush_mapper = MapperRegistry::GetInstance()
                          ->GetSharedPtrMapper<platform::graphics::IBrush>();

  std::shared_ptr<platform::graphics::IBrush> brush;

  for (auto child_node : node->GetChildren()) {
    if (child_node->IsElementNode()) {
      brush = brush_mapper->MapFromXml(child_node->AsElement());
    }
  }

  return ui::style::ContentBrushStyler::Create(std::move(brush));
}

ClonePtr<ui::style::CursorStyler> CursorStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto cursor_mapper = MapperRegistry::GetInstance()
                           ->GetSharedPtrMapper<platform::gui::ICursor>();
  std::shared_ptr<platform::gui::ICursor> cursor;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element &&
        cursor_mapper->XmlElementIsOfThisType(child->AsElement())) {
      cursor = cursor_mapper->MapFromXml(child->AsElement());
    }
  }

  return ui::style::CursorStyler::Create(cursor);
}

ClonePtr<ui::style::FontStyler> FontStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto font_mapper = MapperRegistry::GetInstance()
                         ->GetSharedPtrMapper<platform::graphics::IFont>();

  std::shared_ptr<platform::graphics::IFont> font;

  for (auto child_node : node->GetChildren()) {
    if (child_node->IsElementNode()) {
      font = font_mapper->MapFromXml(child_node->AsElement());
    }
  }

  return ui::style::FontStyler::Create(std::move(font));
}

ClonePtr<ui::style::MarginStyler> MarginStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  Thickness thickness;

  auto thickness_mapper = MapperRegistry::GetInstance()->GetMapper<Thickness>();

  auto value_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attribute) {
    thickness = thickness_mapper->MapFromString(*value_attribute);
  }

  return ui::style::MarginStyler::Create(thickness);
}

ClonePtr<ui::style::PaddingStyler> PaddingStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  Thickness thickness;

  auto thickness_mapper = MapperRegistry::GetInstance()->GetMapper<Thickness>();

  auto value_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("value");
  if (value_attribute) {
    thickness = thickness_mapper->MapFromString(*value_attribute);
  }

  return ui::style::PaddingStyler::Create(thickness);
}

ClonePtr<ui::style::PreferredSizeStyler>
PreferredSizeStylerMapper::DoMapFromXml(xml::XmlElementNode* node) {
  render::MeasureSize size;

  auto measure_length_mapper =
      MapperRegistry::GetInstance()->GetMapper<render::MeasureLength>();

  auto width_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("width");
  if (width_attribute) {
    size.width = measure_length_mapper->MapFromString(*width_attribute);
  }

  auto height_attribute =
      node->GetOptionalAttributeValueCaseInsensitive("height");
  if (height_attribute) {
    size.height = measure_length_mapper->MapFromString(*height_attribute);
  }

  return ui::style::PreferredSizeStyler::Create(size);
}
}  // namespace cru::ui::mapper::style
