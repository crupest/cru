#pragma once
#include "../properties/SelectPropertyEditor.h"
#include "StylerEditor.h"

namespace cru::theme_builder::components::stylers {
class CursorStylerEditor : public StylerEditor {
 public:
  CursorStylerEditor();
  ~CursorStylerEditor() override;

 public:
  ClonablePtr<ui::style::CursorStyler> GetValue();
  void SetValue(ui::style::CursorStyler* styler, bool trigger_change = true);
  void SetValue(const ClonablePtr<ui::style::CursorStyler>& styler,
                bool trigger_change = true) {
    SetValue(styler.get(), trigger_change);
  }

  ClonablePtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  properties::SelectPropertyEditor cursor_select_;
};
}  // namespace cru::theme_builder::components::stylers
