#include "CornerRadiusPropertyEditor.h"
#include "cru/ui/Base.h"

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

  ConnectChangeEvent(left_top_editor_);
  ConnectChangeEvent(right_top_editor_);
  ConnectChangeEvent(left_bottom_editor_);
  ConnectChangeEvent(right_bottom_editor_);
}

CornerRadiusPropertyEditor::~CornerRadiusPropertyEditor() {}

ui::CornerRadius CornerRadiusPropertyEditor::GetValue() const {
  return ui::CornerRadius(
      left_top_editor_.GetValue(), right_top_editor_.GetValue(),
      left_bottom_editor_.GetValue(), right_bottom_editor_.GetValue());
}

void CornerRadiusPropertyEditor::SetValue(const ui::CornerRadius& corner_radius,
                                          bool trigger_change) {
  left_top_editor_.SetValue(corner_radius.left_top, false);
  right_top_editor_.SetValue(corner_radius.right_top, false);
  left_bottom_editor_.SetValue(corner_radius.left_bottom, false);
  right_bottom_editor_.SetValue(corner_radius.right_bottom, false);
  if (trigger_change) RaiseChangeEvent();
}
}  // namespace cru::theme_builder::components::properties
