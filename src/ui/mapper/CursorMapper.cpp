#include "cru/ui/mapper/CursorMapper.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::mapper {
using cru::platform::gui::ICursor;
using cru::platform::gui::SystemCursorType;

std::shared_ptr<ICursor> CursorMapper::DoMapFromString(std::string str) {
  if (str.empty()) return nullptr;

  auto cursor_manager =
      platform::gui::IUiApplication::GetInstance()->GetCursorManager();

  if (cru::string::CaseInsensitiveCompare(str, "arrow") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Arrow);
  } else if (cru::string::CaseInsensitiveCompare(str, "hand") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::Hand);
  } else if (cru::string::CaseInsensitiveCompare(str, "ibeam") == 0) {
    return cursor_manager->GetSystemCursor(SystemCursorType::IBeam);
  } else {
    throw MapException("Unknown cursor name.");
  }
}

std::shared_ptr<ICursor> CursorMapper::DoMapFromXml(xml::XmlElementNode* node) {
  return MapFromXmlAsStringValue(node, nullptr);
}
}  // namespace cru::ui::mapper
