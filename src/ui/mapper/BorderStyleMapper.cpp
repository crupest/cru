#include "cru/ui/mapper/BorderStyleMapper.hpp"
#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
using namespace xml;
using ui::style::ApplyBorderStyleInfo;

bool BorderStyleMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveCompare(u"BorderStyle") == 0;
}

ApplyBorderStyleInfo BorderStyleMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  ApplyBorderStyleInfo result;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == XmlNode::Type::Text) {
      log::Debug(u"Ignore text node.");
    } else {
      auto c = child->AsElement();
      auto thickness_mapper =
          MapperRegistry::GetInstance()->GetMapper<Thickness>();
      auto corner_radius_mapper =
          MapperRegistry::GetInstance()->GetMapper<CornerRadius>();
      auto brush_mapper = MapperRegistry::GetInstance()
                              ->GetRefMapper<platform::graphics::IBrush>();
      if (thickness_mapper->XmlElementIsOfThisType(c)) {
        result.border_thickness = thickness_mapper->MapFromXml(c);
      } else if (corner_radius_mapper->XmlElementIsOfThisType(c)) {
        result.border_radius = corner_radius_mapper->MapFromXml(c);
      } else if (brush_mapper->XmlElementIsOfThisType(c)) {
        auto brush = brush_mapper->MapFromXml(c);
        auto name = c->GetOptionalAttributeCaseInsensitive(u"name");
        if (name) {
          if (name->CaseInsensitiveCompare(u"foreground") == 0) {
            result.foreground_brush = std::move(brush);
          } else if (name->CaseInsensitiveCompare(u"background") == 0) {
            result.background_brush = std::move(brush);
          } else {
            log::Debug(u"Unknown brush name: {}", *name);
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
