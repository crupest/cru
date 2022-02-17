#pragma once
#include "../Editor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
class ConditionEditor : public Editor {
 public:
  ConditionEditor();
  ~ConditionEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::controls::FlexLayout* GetContainer() { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  virtual ClonablePtr<ui::style::Condition> GetCondition() = 0;

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
};

std::unique_ptr<ConditionEditor> CreateConditionEditor(
    ui::style::Condition* condition);
}  // namespace cru::theme_builder::components::conditions
