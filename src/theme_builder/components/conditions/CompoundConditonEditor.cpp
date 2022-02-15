#include "CompoundConditionEditor.h"

namespace cru::theme_builder::components::conditions {
CompoundConditionEditorChild::CompoundConditionEditorChild(
    std::unique_ptr<ConditionEditor>&& condition_editor)
    : condition_editor_(std::move(condition_editor)) {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&remove_button_);

  remove_button_.SetChild(&remove_button_text_);
  remove_button_text_.SetText(u"-");

  container_.AddChild(condition_editor_->GetRootControl());

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { this->remove_event_.Raise(nullptr); });
}

CompoundConditionEditor::CompoundConditionEditor() {
  GetContainer()->AddChild(&children_container_);
  GetContainer()->AddChild(&add_child_button_);

  add_child_button_.SetChild(&add_child_button_text_);
  add_child_button_text_.SetText(u"+");
}

CompoundConditionEditor::~CompoundConditionEditor() {}

}  // namespace cru::theme_builder::components::conditions
