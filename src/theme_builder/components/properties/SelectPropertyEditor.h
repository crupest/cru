#pragma once
#include "../Editor.h"
#include "cru/ui/components/Select.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder::components::properties {
class SelectPropertyEditor : public Editor {
 public:
  using PropertyType = Index;

  SelectPropertyEditor();
  ~SelectPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  Index GetSelectedIndex() const { return select_.GetSelectedIndex(); }
  void SetSelectedIndex(Index index, bool trigger_change = true) {
    if (trigger_change == false) SuppressNextChangeEvent();
    select_.SetSelectedIndex(index);
  }

  std::vector<String> GetItems() const { return select_.GetItems(); }
  void SetItems(std::vector<String> items) {
    select_.SetItems(std::move(items));
  }

  Index GetValue() const { return GetSelectedIndex(); }
  void SetValue(Index value, bool trigger_change = true) {
    SetSelectedIndex(value, trigger_change);
  }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::components::Select select_;
};
}  // namespace cru::theme_builder::components::properties
