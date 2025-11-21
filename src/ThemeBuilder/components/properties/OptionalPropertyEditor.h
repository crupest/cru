#pragma once
#include "../Editor.h"
#include "../LabeledMixin.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"

#include <optional>

namespace cru::theme_builder::components::properties {
template <typename TEditor>
class OptionalPropertyEditor : public Editor, public LabeledMixin {
 public:
  using PropertyType = typename TEditor::PropertyType;

  OptionalPropertyEditor() {
    container_.AddChild(&label_);
    container_.AddChild(&check_box_);
    check_box_.SetMargin({0, 0, 10, 0});
    container_.AddChild(editor_.GetRootControl());

    editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
      if (IsEnabled()) {
        RaiseChangeEvent();
      }
    });
  }
  ~OptionalPropertyEditor() override {}

  ui::controls::Control* GetRootControl() override { return &container_; }

  bool IsEnabled() { return check_box_.IsChecked(); }
  void SetEnabled(bool enabled, bool trigger_change = true) {
    check_box_.SetChecked(enabled);
    if (trigger_change) {
      RaiseChangeEvent();
    }
  }

  std::optional<PropertyType> GetValue() {
    return IsEnabled() ? std::optional<PropertyType>(editor_.GetValue())
                       : std::nullopt;
  }

  void SetValue(std::optional<PropertyType> value, bool trigger_change = true) {
    if (value) {
      SetEnabled(true, false);
      editor_.SetValue(*value, false);
      if (trigger_change) RaiseChangeEvent();
    } else {
      SetEnabled(false, trigger_change);
    }
  }

  TEditor* GetEditor() { return &editor_; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::CheckBox check_box_;
  TEditor editor_;
};
}  // namespace cru::theme_builder::components::properties
