#include "cru/ui/style/Styler.hpp"

#include "../Helper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/controls/Control.hpp"
#include "cru/ui/controls/IBorderControl.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"

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
