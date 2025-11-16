#include "cru/ui/mapper/style/CursorStylerMapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
bool CursorStylerMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "CursorStyler") == 0;
}

ClonePtr<ui::style::CursorStyler> CursorStylerMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto cursor_mapper =
      MapperRegistry::GetInstance()->GetSharedPtrMapper<platform::gui::ICursor>();
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
