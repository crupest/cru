#include "cru/ui/mapper/style/BorderStyleMapper.hpp"
#include "../../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
using namespace xml;
using ui::style::ApplyBorderStyleInfo;

bool BorderStyleMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag() == u"BorderStyle";
}

std::unique_ptr<ApplyBorderStyleInfo> BorderStyleMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto result = std::make_unique<ApplyBorderStyleInfo>();

  for (auto child : node->GetChildren()) {
    if (child->GetType() == XmlNode::Type::Text) {
      log::Debug(u"Ignore text node.");
    } else {
      auto c = child->AsElement();
      auto thickness_mapper =
          MapperRegistry::GetInstance()->GetMapper<Thickness>();
      auto corner_radius_mapper =
          MapperRegistry::GetInstance()->GetMapper<CornerRadius>();
      auto color_mapper = MapperRegistry::GetInstance()->GetMapper<Color>();
      if (thickness_mapper->XmlElementIsOfThisType(c)) {
        result->border_thickness = *thickness_mapper->MapFromXml(c);
      } else if (corner_radius_mapper->XmlElementIsOfThisType(c)) {
        result->border_radius = *corner_radius_mapper->MapFromXml(c);
      } else if (color_mapper->XmlElementIsOfThisType(c)) {
        auto brush = GetGraphicsFactory()->CreateSolidColorBrush(
            *color_mapper->MapFromXml(c));
        auto name = c->GetOptionalAttribute(u"name");
        if (name) {
          if (name == u"foreground") {
            result->foreground_brush = std::move(brush);
          } else if (name == u"background") {
            result->background_brush = std::move(brush);
          } else {
            log::Debug(u"Unknown brush name: {}", *name);
          }
        } else {
          result->border_brush = std::move(brush);
        }
      }
    }
  }

  return result;
}
}  // namespace cru::ui::mapper::style
