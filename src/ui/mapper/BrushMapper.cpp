#include "cru/ui/mapper/BrushMapper.h"
#include "cru/base/xml/XmlNode.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/mapper/Mapper.h"
#include "cru/ui/mapper/MapperRegistry.h"

#include <memory>

namespace cru::ui::mapper {
bool BrushMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  auto color_mapper = MapperRegistry::GetInstance()->GetMapper<Color>();
  return color_mapper->XmlElementIsOfThisType(node) || node->HasTag("Brush");
}

std::shared_ptr<platform::graphics::IBrush> BrushMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto graphics_factory =
      platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory();
  auto color_mapper = MapperRegistry::GetInstance()->GetMapper<Color>();

  try {
    auto color = color_mapper->MapFromXml(node);
    return graphics_factory->CreateSolidColorBrush(color);
  } catch (const MapException&) {
  }

  for (auto child : node->GetChildren()) {
    if (auto c = child->AsElement()) {
      if (color_mapper->XmlElementIsOfThisType(node)) {
        auto color = color_mapper->MapFromXml(node);
        return graphics_factory->CreateSolidColorBrush(color);
      } else {
        throw MapException("Invalid child element of Brush.");
      }
    } else if (child->IsTextNode()) {
      throw MapException("Text node is not allowed in Brush.");
    }
  }

  throw MapException("Brush doesn't have content.");
}
}  // namespace cru::ui::mapper
