#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder {
class ConditionEditor : public ui::components::Component {
 public:
  ConditionEditor();

  ~ConditionEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return nullptr; }

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
};
}  // namespace cru::theme_builder
