#include "cru/ui/datamodel/CursorDataType.h"

#include "cru/base/StringUtil.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui::datamodel {
using cru::platform::gui::ICursor;
using cru::platform::gui::SystemCursorType;

CursorDataType::CursorDataType()
    : SharedPtrDataTypeBase<platform::gui::ICursor>(
          "Cursor", {true, false, false, false}) {}

DataConvertResult<std::shared_ptr<ICursor>> CursorDataType::DoConvertFromString(
    std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<std::shared_ptr<ICursor>>::Success(nullptr);
  }

  auto cursor_manager =
      platform::gui::IUiApplication::GetInstance()->GetCursorManager();

  if (cru::string::CaseInsensitiveCompare(value, "arrow") == 0) {
    return DataConvertResult<std::shared_ptr<ICursor>>::Success(
        cursor_manager->GetSystemCursor(SystemCursorType::Arrow));
  }
  if (cru::string::CaseInsensitiveCompare(value, "hand") == 0) {
    return DataConvertResult<std::shared_ptr<ICursor>>::Success(
        cursor_manager->GetSystemCursor(SystemCursorType::Hand));
  }
  if (cru::string::CaseInsensitiveCompare(value, "ibeam") == 0) {
    return DataConvertResult<std::shared_ptr<ICursor>>::Success(
        cursor_manager->GetSystemCursor(SystemCursorType::IBeam));
  }

  return DataConvertResult<std::shared_ptr<ICursor>>::Failure(
      "Unknown cursor name.");
}
}  // namespace cru::ui::datamodel
