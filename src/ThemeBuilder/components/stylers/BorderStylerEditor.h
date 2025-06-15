#pragma once
#include "../properties/ColorPropertyEditor.h"
#include "../properties/CornerRadiusPropertyEditor.h"
#include "../properties/OptionalPropertyEditor.h"
#include "../properties/ThicknessPropertyEditor.h"
#include "StylerEditor.h"
#include <cru/CopyPtr.h>

namespace cru::theme_builder::components::stylers {
class BorderStylerEditor : public StylerEditor {
 public:
  BorderStylerEditor();
  ~BorderStylerEditor() override;

  CopyPtr<ui::style::BorderStyler> GetValue();
  void SetValue(ui::style::BorderStyler* styler, bool trigger_change = true);
  void SetValue(const CopyPtr<ui::style::BorderStyler>& styler,
                bool trigger_change = true) {
    SetValue(styler.get(), trigger_change);
  }

  CopyPtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  properties::OptionalPropertyEditor<properties::CornerRadiusPropertyEditor>
      corner_radius_editor_;
  properties::OptionalPropertyEditor<properties::ThicknessPropertyEditor>
      thickness_editor_;
  properties::OptionalPropertyEditor<properties::ColorPropertyEditor>
      brush_editor_;
  properties::OptionalPropertyEditor<properties::ColorPropertyEditor>
      foreground_brush_editor_;
  properties::OptionalPropertyEditor<properties::ColorPropertyEditor>
      background_brush_editor_;
};
}  // namespace cru::theme_builder::components::stylers
