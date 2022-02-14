#include "CornerRadiusPropertyEditor.h"

namespace cru::theme_builder::components::properties {
CornerRadiusPropertyEditor::CornerRadiusPropertyEditor() {
  left_top_editor_.SetLabel(u"Left Top");
  right_top_editor_.SetLabel(u"Right Top");
  left_bottom_editor_.SetLabel(u"Left Bottom");
  right_bottom_editor_.SetLabel(u"Right Bottom");

  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(left_top_editor_.GetRootControl());
  container_.AddChild(right_top_editor_.GetRootControl());
  container_.AddChild(left_bottom_editor_.GetRootControl());
  container_.AddChild(right_bottom_editor_.GetRootControl());

  left_top_editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
    corner_radius_.left_top = left_top_editor_.GetValue();
    change_event_.Raise(nullptr);
  });

  right_top_editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
    corner_radius_.right_top = left_top_editor_.GetValue();
    change_event_.Raise(nullptr);
  });

  left_bottom_editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
    corner_radius_.left_bottom = left_bottom_editor_.GetValue();
    change_event_.Raise(nullptr);
  });

  right_bottom_editor_.ChangeEvent()->AddHandler([this](std::nullptr_t) {
    corner_radius_.right_bottom = right_bottom_editor_.GetValue();
    change_event_.Raise(nullptr);
  });
}

CornerRadiusPropertyEditor::~CornerRadiusPropertyEditor() {
  container_.RemoveFromParent();
}

void CornerRadiusPropertyEditor::SetValue(const ui::CornerRadius& corner_radius,
                                          bool trigger_change) {
  left_top_editor_.SetValue(corner_radius_.left_top, false);
  right_top_editor_.SetValue(corner_radius_.right_top, false);
  left_bottom_editor_.SetValue(corner_radius_.left_bottom, false);
  right_bottom_editor_.SetValue(corner_radius_.right_bottom, false);
  if (trigger_change) change_event_.Raise(nullptr);
}
}  // namespace cru::theme_builder::components::properties
