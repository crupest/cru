#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder::components::conditions {
class ConditionEditor : public ui::components::Component {
 public:
  ConditionEditor();

  ~ConditionEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::controls::FlexLayout* GetContainer() { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
};
}  // namespace cru::theme_builder::components::conditions
