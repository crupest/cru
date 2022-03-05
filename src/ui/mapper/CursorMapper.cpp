#include "cru/ui/mapper/CursorMapper.h"
#include "../Helper.h"
#include "cru/common/Exception.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::mapper {
using cru::platform::gui::ICursor;
using cru::platform::gui::SystemCursorType;

bool CursorMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  return node->GetTag().CaseInsensitiveCompare(u"Cursor") == 0;
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromString(String str) {
  if (str.empty()) return nullptr;

  auto cursor_manager = GetUiApplication()->GetCursorManager();

  if (str.CaseInsensitiveCompare(u"arrow") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Arrow);
  } else if (str.CaseInsensitiveCompare(u"hand") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Hand);
  } else if (str.CaseInsensitiveCompare(u"ibeam") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::IBeam);
  } else {
    throw Exception(u"Unsupported cursor type.");
  }
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromXml(xml::XmlElementNode *node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive(u"value");
  if (!value_attr) return nullptr;
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
