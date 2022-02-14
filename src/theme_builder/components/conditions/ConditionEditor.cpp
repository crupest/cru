#include "ConditionEditor.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::conditions {
ConditionEditor::ConditionEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(&label_);
}

ConditionEditor::~ConditionEditor() { container_.RemoveFromParent(); }
}  // namespace cru::theme_builder::components::conditions
