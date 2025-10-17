#include "cru/ui/mapper/style/FontStylerMapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/mapper/MapperRegistry.h"

namespace cru::ui::mapper::style {
FontStylerMapper::FontStylerMapper() { SetAllowedTags({"FontStyler"}); }

FontStylerMapper::~FontStylerMapper() {}

ClonablePtr<ui::style::FontStyler> FontStylerMapper::DoMapFromXml(
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
}  // namespace cru::ui::mapper::style
