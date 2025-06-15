#include "cru/ui/mapper/style/FontStylerMapper.h"
#include <cru/CopyPtr.h>
#include "cru/ui/mapper/MapperRegistry.h"

namespace cru::ui::mapper::style {
FontStylerMapper::FontStylerMapper() { SetAllowedTags({u"FontStyler"}); }

FontStylerMapper::~FontStylerMapper() {}

CopyPtr<ui::style::FontStyler> FontStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto font_mapper = MapperRegistry::GetInstance()
                         ->GetSharedPtrMapper<graphics::IFont>();

  std::shared_ptr<graphics::IFont> font;

  for (auto child_node : node->GetChildren()) {
    if (child_node->IsElementNode()) {
      font = font_mapper->MapFromXml(child_node->AsElement());
    }
  }

  return ui::style::FontStyler::Create(std::move(font));
}
}  // namespace cru::ui::mapper::style
