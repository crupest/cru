#include "cru/ui/mapper/CursorMapper.hpp"
#include "../Helper.hpp"
#include "cru/common/Exception.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"

namespace cru::ui::mapper {
using cru::platform::gui::ICursor;
using cru::platform::gui::SystemCursorType;

bool CursorMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  return node->GetTag() == u"Cursor";
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromString(String str) {
  if (str.empty()) return nullptr;

  auto cursor_manager = GetUiApplication()->GetCursorManager();

  if (str == u"arrow") {
    return cursor_manager->GetSystemCursor(SystemCursorType::Arrow);
  } else if (str == u"hand") {
    return cursor_manager->GetSystemCursor(SystemCursorType::Hand);
  } else if (str == u"ibeam") {
    return cursor_manager->GetSystemCursor(SystemCursorType::IBeam);
  } else {
    throw Exception(u"Unsupported cursor type.");
  }
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromXml(xml::XmlElementNode *node) {
  auto value_attr = node->GetOptionalAttribute(u"value");
  if (!value_attr) return nullptr;
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
