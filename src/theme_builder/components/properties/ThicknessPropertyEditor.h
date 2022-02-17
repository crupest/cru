#pragma once
#include "../Editor.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class ThicknessPropertyEditor : public Editor {
 public:
  using PropertyType = ui::Thickness;

  ThicknessPropertyEditor();
  ~ThicknessPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  ui::Thickness GetValue() const { return thickness_; }
  void SetValue(const ui::Thickness& thickness, bool trigger_change = true);

 private:
  ui::Thickness thickness_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox text_;
  bool is_text_valid_;
};
}  // namespace cru::theme_builder::components::properties
