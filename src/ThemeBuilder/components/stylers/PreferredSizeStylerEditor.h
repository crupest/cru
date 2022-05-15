#pragma once
#include "../properties/MeasureLengthPropertyEditor.h"
#include "StylerEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class PreferredSizeStylerEditor : public StylerEditor {
 public:
  PreferredSizeStylerEditor();
  ~PreferredSizeStylerEditor() override;

  ClonablePtr<ui::style::PreferredSizeStyler> GetValue();
  void SetValue(ui::style::PreferredSizeStyler* styler,
                bool trigger_change = true);
  void SetValue(const ClonablePtr<ui::style::PreferredSizeStyler>& styler,
                bool trigger_change = true) {
    SetValue(styler.get(), trigger_change);
  }

  ClonablePtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  properties::MeasureLengthPropertyEditor width_editor_;
  properties::MeasureLengthPropertyEditor height_editor_;
};
}  // namespace cru::theme_builder::components::stylers
