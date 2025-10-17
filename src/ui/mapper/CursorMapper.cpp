#include "cru/ui/mapper/CursorMapper.h"
#include "../Helper.h"
#include "cru/base/Exception.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::mapper {
using cru::platform::gui::ICursor;
using cru::platform::gui::SystemCursorType;

bool CursorMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "Cursor") == 0;
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromString(std::string str) {
  if (str.empty()) return nullptr;

  auto cursor_manager = GetUiApplication()->GetCursorManager();

  if (cru::string::CaseInsensitiveCompare(str, "arrow") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Arrow);
  } else if (cru::string::CaseInsensitiveCompare(str, "hand") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Hand);
  } else if (cru::string::CaseInsensitiveCompare(str, "ibeam") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::IBeam);
  } else {
    throw Exception("Unsupported cursor type.");
  }
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromXml(xml::XmlElementNode *node) {
  auto value_attr = node->GetOptionalAttributeValueCaseInsensitive("value");
  if (!value_attr) return nullptr;
  return DoMapFromString(*value_attr);
}
}  // namespace cru::ui::mapper
