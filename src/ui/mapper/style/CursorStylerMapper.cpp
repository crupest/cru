#include "cru/ui/mapper/style/CursorStylerMapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/Styler.hpp"

namespace cru::ui::mapper::style {
bool CursorStylerMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return node->GetTag().CaseInsensitiveEqual(u"CursorStyler");
}

ClonablePtr<ui::style::CursorStyler> CursorStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto cursor_mapper =
      MapperRegistry::GetInstance()->GetRefMapper<platform::gui::ICursor>();
  std::shared_ptr<platform::gui::ICursor> cursor;

  for (auto child : node->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element &&
        cursor_mapper->XmlElementIsOfThisType(child->AsElement())) {
      cursor = cursor_mapper->MapFromXml(child->AsElement());
    }
  }

  return ui::style::CursorStyler::Create(cursor);
}
}  // namespace cru::ui::mapper::style
