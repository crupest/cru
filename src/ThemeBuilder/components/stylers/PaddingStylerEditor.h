#pragma once
#include "../properties/ThicknessPropertyEditor.h"
#include "StylerEditor.h"
#include <cru/CopyPtr.h>
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class PaddingStylerEditor : public StylerEditor {
 public:
  PaddingStylerEditor();
  ~PaddingStylerEditor() override;

  CopyPtr<ui::style::PaddingStyler> GetValue();
  void SetValue(ui::style::PaddingStyler* styler, bool trigger_change = true);
  void SetValue(const CopyPtr<ui::style::PaddingStyler>& styler,
                bool trigger_change = true) {
    SetValue(styler.get(), trigger_change);
  }

  CopyPtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  properties::ThicknessPropertyEditor thickness_editor_;
};
}  // namespace cru::theme_builder::components::stylers
