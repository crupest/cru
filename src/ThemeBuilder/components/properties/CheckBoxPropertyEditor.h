#pragma once
#include "../Editor.h"
#include "../LabeledMixin.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
class CheckBoxPropertyEditor : public Editor, public LabeledMixin {
 public:
  using PropertyType = bool;

  CheckBoxPropertyEditor();
  ~CheckBoxPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  bool GetValue() { return check_box_.IsChecked(); }
  void SetValue(bool value, bool trigger_change = true);

 private:
  ui::controls::FlexLayout container_;
  ui::controls::CheckBox check_box_;
};
}  // namespace cru::theme_builder::components::properties
