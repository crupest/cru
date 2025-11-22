#include "cru/ui/style/Styler.h"

#include "cru/base/ClonePtr.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/IBorderControl.h"
#include "cru/ui/controls/IContentBrushControl.h"
#include "cru/ui/controls/IFontControl.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"

namespace cru::ui::style {
BorderStyler::BorderStyler(ApplyBorderStyleInfo style)
    : style_(std::move(style)) {}

void BorderStyler::Apply(controls::Control* control) const {
  if (auto border_control = dynamic_cast<controls::IBorderControl*>(control)) {
    border_control->ApplyBorderStyle(style_);
  }
}

ClonePtr<CursorStyler> CursorStyler::Create(
    platform::gui::SystemCursorType type) {
  return Create(platform::gui::IUiApplication::GetInstance()
                    ->GetCursorManager()
                    ->GetSystemCursor(type));
}

void CursorStyler::Apply(controls::Control* control) const {
  control->SetCursor(cursor_);
}

void PreferredSizeStyler::Apply(controls::Control* control) const {
  control->SetSuggestSize(size_);
}

void MarginStyler::Apply(controls::Control* control) const {
  control->SetMargin(margin_);
}

void PaddingStyler::Apply(controls::Control* control) const {
  control->SetPadding(padding_);
}

void ContentBrushStyler::Apply(controls::Control* control) const {
  if (auto content_brush_control =
          dynamic_cast<controls::IContentBrushControl*>(control)) {
    content_brush_control->SetContentBrush(brush_);
  }
}

void FontStyler::Apply(controls::Control* control) const {
  if (auto font_control = dynamic_cast<controls::IFontControl*>(control)) {
    font_control->SetFont(font_);
  }
}
}  // namespace cru::ui::style
