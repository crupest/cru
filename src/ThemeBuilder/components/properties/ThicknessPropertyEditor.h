#pragma once
#include "../Editor.h"
#include "../LabeledMixin.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class ThicknessPropertyEditor : public Editor, public LabeledMixin {
 public:
  using PropertyType = ui::Thickness;

  ThicknessPropertyEditor();
  ~ThicknessPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::Thickness GetValue() { return thickness_; }
  void SetValue(const ui::Thickness& thickness, bool trigger_change = true);

 private:
  ui::Thickness thickness_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBox text_;
  bool is_text_valid_;
};
}  // namespace cru::theme_builder::components::properties
