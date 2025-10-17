#pragma once
#include "../Editor.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder::components::properties {
class CheckBoxPropertyEditor : public Editor {
 public:
  using PropertyType = bool;

  CheckBoxPropertyEditor();
  ~CheckBoxPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  std::string GetLabel() const { return label_.GetText(); }
  void SetLabel(std::string label) { label_.SetText(std::move(label)); }

  bool GetValue() const { return check_box_.IsChecked(); }
  void SetValue(bool value, bool trigger_change = true);

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::CheckBox check_box_;
};
}  // namespace cru::theme_builder::components::properties
