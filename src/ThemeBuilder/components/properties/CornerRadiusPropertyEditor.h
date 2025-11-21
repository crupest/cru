#pragma once
#include "../Editor.h"
#include "PointPropertyEditor.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
class CornerRadiusPropertyEditor : public Editor {
 public:
  using PropertyType = ui::CornerRadius;

  CornerRadiusPropertyEditor();
  ~CornerRadiusPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::CornerRadius GetValue();
  void SetValue(const ui::CornerRadius& corner_radius,
                bool trigger_change = true);

 private:
  ui::controls::FlexLayout container_;
  PointPropertyEditor left_top_editor_;
  PointPropertyEditor right_top_editor_;
  PointPropertyEditor left_bottom_editor_;
  PointPropertyEditor right_bottom_editor_;
};
}  // namespace cru::theme_builder::components::properties
