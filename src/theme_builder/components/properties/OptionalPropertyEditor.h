#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
template <typename TEditor>
class OptionalPropertyEditor : public ui::components::Component {
 public:
  OptionalPropertyEditor() {
    container_.AddChild(&check_box_);
    container_.AddChild(editor_->GetRootControl());
  }
  ~OptionalPropertyEditor() override { container_.RemoveFromParent(); }

  ui::controls::Control* GetRootControl() override { return &container_; }

  bool IsEnabled() const { return check_box_.IsChecked(); }
  void SetEnabled(bool enabled) { check_box_.SetChecked(enabled); }

  TEditor* GetEditor() { return &editor_; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::CheckBox check_box_;
  TEditor editor_;
};
}  // namespace cru::theme_builder::components::properties
