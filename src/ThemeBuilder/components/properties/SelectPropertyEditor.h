#pragma once
#include "../Editor.h"
#include "../LabeledMixin.h"
#include "cru/ui/components/Select.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
class SelectPropertyEditor : public Editor, public LabeledMixin {
 public:
  using PropertyType = Index;

  SelectPropertyEditor();
  ~SelectPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  Index GetSelectedIndex() { return select_.GetSelectedIndex(); }
  void SetSelectedIndex(Index index, bool trigger_change = true) {
    if (trigger_change == false) SuppressNextChangeEvent();
    select_.SetSelectedIndex(index);
  }

  std::vector<std::string> GetItems() { return select_.GetItems(); }
  void SetItems(std::vector<std::string> items) {
    select_.SetItems(std::move(items));
  }

  Index GetValue() { return GetSelectedIndex(); }
  void SetValue(Index value, bool trigger_change = true) {
    SetSelectedIndex(value, trigger_change);
  }

 private:
  ui::controls::FlexLayout container_;
  ui::components::Select select_;
};
}  // namespace cru::theme_builder::components::properties
