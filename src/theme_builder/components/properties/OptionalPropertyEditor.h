#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/CheckBox.h"
#include "cru/ui/controls/FlexLayout.h"

#include <optional>

namespace cru::theme_builder::components::properties {
template <typename TEditor>
class OptionalPropertyEditor : public ui::components::Component {
 public:
  using PropertyType = typename TEditor::PropertyType;

  OptionalPropertyEditor() {
    container_.AddChild(&check_box_);
    container_.AddChild(editor_.GetRootControl());

    editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
      if (IsEnabled()) {
        change_event_.Raise(nullptr);
      }
    });
  }
  ~OptionalPropertyEditor() override {}

  ui::controls::Control* GetRootControl() override { return &container_; }

  bool IsEnabled() const { return check_box_.IsChecked(); }
  void SetEnabled(bool enabled, bool trigger_change = true) {
    check_box_.SetChecked(enabled);
    if (trigger_change) {
      change_event_.Raise(nullptr);
    }
  }

  std::optional<PropertyType> GetValue() const {
    return IsEnabled() ? std::optional<PropertyType>(editor_.GetValue())
                       : std::nullopt;
  }

  void SetValue(std::optional<PropertyType> value, bool trigger_change = true) {
    if (value) {
      SetEnabled(true, false);
      editor_.SetValue(*value, false);
      if (trigger_change) change_event_.Raise(nullptr);
    } else {
      SetEnabled(false, trigger_change);
    }
  }

  TEditor* GetEditor() { return &editor_; }

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::CheckBox check_box_;
  TEditor editor_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::properties
