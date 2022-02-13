#pragma once
#include "../properties/ColorPropertyEditor.h"
#include "../properties/CornerRadiusPropertyEditor.h"
#include "../properties/OptionalPropertyEditor.h"
#include "../properties/ThicknessPropertyEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::styler {
class BorderStylerEditor : public ui::components::Component {
 public:
  BorderStylerEditor();
  ~BorderStylerEditor() override;

  ui::controls::Control* GetRootControl() override { return nullptr; }

  ClonablePtr<ui::style::BorderStyler> GetStyler();
  void SetStyler(const ClonablePtr<ui::style::BorderStyler>& styler);

  IEvent<ClonablePtr<ui::style::BorderStyler>>* ChangeEvent() {
    return &change_event_;
  }

 private:
  ui::controls::FlexLayout container_;
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

  Event<ClonablePtr<ui::style::BorderStyler>> change_event_;
};
}  // namespace cru::theme_builder::components::styler
