#include "SelectPropertyEditor.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
SelectPropertyEditor::SelectPropertyEditor() {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&label_);
  container_.AddChild(select_.GetRootControl());

  select_.ItemSelectedEvent()->AddHandler([this](Index index) {
    if (!suppress_next_change_event_) {
      change_event_.Raise(nullptr);
    } else {
      suppress_next_change_event_ = false;
    }
  });
}

SelectPropertyEditor::~SelectPropertyEditor() { container_.RemoveFromParent(); }

}  // namespace cru::theme_builder::components::properties
