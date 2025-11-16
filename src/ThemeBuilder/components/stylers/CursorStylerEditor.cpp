#include "CursorStylerEditor.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::theme_builder::components::stylers {
CursorStylerEditor::CursorStylerEditor() {
  SetLabel("Cursor Styler");
  GetContainer()->AddChild(cursor_select_.GetRootControl());

  cursor_select_.SetLabel("Cursor");
  cursor_select_.SetItems({"arrow", "hand", "ibeam"});
  cursor_select_.SetSelectedIndex(0);

  ConnectChangeEvent(cursor_select_);
}

CursorStylerEditor::~CursorStylerEditor() {}

ClonePtr<ui::style::CursorStyler> CursorStylerEditor::GetValue() {
  auto cursor_manager =
      platform::gui::IUiApplication::GetInstance()->GetCursorManager();

  std::shared_ptr<platform::gui::ICursor> cursor;

  switch (cursor_select_.GetSelectedIndex()) {
    case 0:
      cursor = cursor_manager->GetSystemCursor(
          platform::gui::SystemCursorType::Arrow);
      break;
    case 1:
      cursor = cursor_manager->GetSystemCursor(
          platform::gui::SystemCursorType::Hand);
      break;
    case 2:
      cursor = cursor_manager->GetSystemCursor(
          platform::gui::SystemCursorType::IBeam);
      break;
  }

  return ui::style::CursorStyler::Create(cursor);
}

void CursorStylerEditor::SetValue(ui::style::CursorStyler* styler,
                                  bool trigger_change) {
  auto cursor_manager =
      platform::gui::IUiApplication::GetInstance()->GetCursorManager();

  auto cursor = styler->GetCursor();

  if (cursor ==
      cursor_manager->GetSystemCursor(platform::gui::SystemCursorType::Arrow)) {
    cursor_select_.SetSelectedIndex(0);
  } else if (cursor == cursor_manager->GetSystemCursor(
                           platform::gui::SystemCursorType::Hand)) {
    cursor_select_.SetSelectedIndex(1);
  } else if (cursor == cursor_manager->GetSystemCursor(
                           platform::gui::SystemCursorType::IBeam)) {
    cursor_select_.SetSelectedIndex(2);
  }

  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::stylers
