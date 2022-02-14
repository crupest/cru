#include "CompoundConditionEditor.h"

namespace cru::theme_builder::components::conditions {
CompoundConditionEditor::CompoundConditionEditor() {
  GetContainer()->AddChild(&children_container_);
  GetContainer()->AddChild(&add_child_button_);

  add_child_button_.SetChild(&add_child_button_text_);
  add_child_button_text_.SetText(u"Add");
}

CompoundConditionEditor::~CompoundConditionEditor() {}

}  // namespace cru::theme_builder::components::conditions
