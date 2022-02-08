#include "cru/ui/style/Styler.h"

#include "../Helper.h"
#include "cru/common/ClonablePtr.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/IBorderControl.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"

namespace cru::ui::style {
BorderStyler::BorderStyler(ApplyBorderStyleInfo style)
    : style_(std::move(style)) {}

void BorderStyler::Apply(controls::Control *control) const {
  if (auto border_control = dynamic_cast<controls::IBorderControl *>(control)) {
    border_control->ApplyBorderStyle(style_);
  }
}

ClonablePtr<CursorStyler> CursorStyler::Create(
    platform::gui::SystemCursorType type) {
  return Create(GetUiApplication()->GetCursorManager()->GetSystemCursor(type));
}

void CursorStyler::Apply(controls::Control *control) const {
  control->SetCursor(cursor_);
}
}  // namespace cru::ui::style
