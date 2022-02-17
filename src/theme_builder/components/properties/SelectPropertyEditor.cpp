#include "SelectPropertyEditor.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
SelectPropertyEditor::SelectPropertyEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&label_);
  container_.AddChild(select_.GetRootControl());

  select_.ItemSelectedEvent()->AddHandler(
      [this](Index index) { RaiseChangeEvent(); });
}

SelectPropertyEditor::~SelectPropertyEditor() {}
}  // namespace cru::theme_builder::components::properties
