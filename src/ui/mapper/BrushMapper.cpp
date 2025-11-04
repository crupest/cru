#include "cru/ui/mapper/BrushMapper.h"
#include "../Helper.h"
#include "cru/base/StringUtil.h"
#include "cru/platform/Color.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/base/xml/XmlNode.h"

#include <memory>

namespace cru::ui::mapper {
bool BrushMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  auto color_mapper = MapperRegistry::GetInstance()->GetMapper<Color>();
  return color_mapper->XmlElementIsOfThisType(node) ||
         cru::string::CaseInsensitiveCompare(node->GetTag(), "Brush") == 0;
}

std::shared_ptr<platform::graphics::IBrush> BrushMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto color_mapper = MapperRegistry::GetInstance()->GetMapper<Color>();

  Color color = colors::transparent;

  if (color_mapper->XmlElementIsOfThisType(node)) {
    color = color_mapper->MapFromXml(node);
  } else {
    for (auto child : node->GetChildren()) {
      if (child->IsElementNode()) {
        auto c = child->AsElement();
        if (color_mapper->XmlElementIsOfThisType(node)) {
          color = color_mapper->MapFromXml(node);
        }
      }
    }
  }

  return GetGraphicsFactory()->CreateSolidColorBrush(color);
}
}  // namespace cru::ui::mapper
