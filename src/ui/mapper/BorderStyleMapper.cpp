#include "cru/ui/mapper/BorderStyleMapper.h"
#include "cru/base/StringUtil.h"
#include "cru/base/xml/XmlNode.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"

namespace cru::ui::mapper {
using namespace xml;
using cru::string::CaseInsensitiveCompare;
using ui::style::ApplyBorderStyleInfo;

ApplyBorderStyleInfo BorderStyleMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  ApplyBorderStyleInfo result;

  for (auto child : node->GetChildren()) {
    if (child->IsTextNode()) {
      throw MapException("BorderStyle can't have text nodes.");
    } else if (child->IsElementNode()) {
      auto c = child->AsElement();
      auto thickness_mapper =
          MapperRegistry::GetInstance()->GetMapper<Thickness>();
      auto corner_radius_mapper =
          MapperRegistry::GetInstance()->GetMapper<CornerRadius>();
      auto brush_mapper =
          MapperRegistry::GetInstance()
              ->GetSharedPtrMapper<platform::graphics::IBrush>();
      if (thickness_mapper->XmlElementIsOfThisType(c)) {
        result.border_thickness = thickness_mapper->MapFromXml(c);
      } else if (corner_radius_mapper->XmlElementIsOfThisType(c)) {
        result.border_radius = corner_radius_mapper->MapFromXml(c);
      } else if (brush_mapper->XmlElementIsOfThisType(c)) {
        auto brush = brush_mapper->MapFromXml(c);
        auto name = c->GetOptionalAttributeValueCaseInsensitive("name");
        if (name) {
          if (CaseInsensitiveCompare(*name, "foreground") == 0) {
            result.foreground_brush = std::move(brush);
          } else if (CaseInsensitiveCompare(*name, "background") == 0) {
            result.background_brush = std::move(brush);
          } else {
            throw MapException("Invalid name of brush in BorderStyle.");
          }
        } else {
          result.border_brush = std::move(brush);
        }
      }
    }
  }

  return result;
}
}  // namespace cru::ui::mapper
