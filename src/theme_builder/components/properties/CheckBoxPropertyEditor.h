#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder::components::properties {
class CheckBoxPropertyEditor : public ui::components::Component {
 public:
  using PropertyType = bool;

  CheckBoxPropertyEditor();
  ~CheckBoxPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  bool GetValue() const { return check_box_.IsChecked(); }
  void SetValue(bool value, bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::CheckBox check_box_;

  bool suppress_next_change_event_ = false;
  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::properties
