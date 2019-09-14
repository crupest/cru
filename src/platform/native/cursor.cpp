#include "cru/platform/native/cursor.hpp"

#include "cru/platform/native/ui_application.hpp"

namespace cru::platform::native {
std::shared_ptr<Cursor> GetSystemCursor(SystemCursor type) {
  return UiApplication::GetInstance()->GetCursorManager()->GetSystemCursor(
      type);
}
}  // namespace cru::platform::native
