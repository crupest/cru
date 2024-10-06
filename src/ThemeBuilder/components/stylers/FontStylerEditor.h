#pragma once
#include "../Editor.h"
#include "../properties/FontPropertyEditor.h"
#include "StylerEditor.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class FontStylerEditor : public StylerEditor {
 public:
  FontStylerEditor();
  ~FontStylerEditor();

 public:
  ClonablePtr<ui::style::FontStyler> GetValue() const;
  void SetValue(ui::style::FontStyler* value, bool trigger_change = true);

  ClonablePtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  properties::FontPropertyEditor font_editor_;
};
}  // namespace cru::theme_builder::components::stylers
