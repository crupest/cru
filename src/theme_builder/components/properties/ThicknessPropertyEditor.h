#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class ThicknessPropertyEditor : public ui::components::Component {
 public:
  using PropertyType = ui::Thickness;

  ThicknessPropertyEditor();
  ~ThicknessPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  ui::Thickness GetValue() const { return thickness_; }
  void SetValue(const ui::Thickness& thickness, bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  ui::Thickness thickness_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox text_;
  bool is_text_valid_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::properties