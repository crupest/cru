#include "cru/ui/mapper/style/ContentBrushStylerMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/Styler.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
ContentBrushStylerMapper::ContentBrushStylerMapper() {
  SetAllowedTags({"ContentBrushStyler"});
}

ContentBrushStylerMapper::~ContentBrushStylerMapper() {}

ClonePtr<ui::style::ContentBrushStyler>
ContentBrushStylerMapper::DoMapFromXml(xml::XmlElementNode* node) {
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
}  // namespace cru::ui::mapper::style
