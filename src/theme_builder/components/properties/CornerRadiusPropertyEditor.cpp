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

  left_top_editor_.PointChangeEvent()->AddHandler(
      [this](const ui::Point& point) {
        corner_radius_.left_top = point;
        corner_radius_change_event_.Raise(corner_radius_);
      });

  right_top_editor_.PointChangeEvent()->AddHandler(
      [this](const ui::Point& point) {
        corner_radius_.right_top = point;
        corner_radius_change_event_.Raise(corner_radius_);
      });

  left_bottom_editor_.PointChangeEvent()->AddHandler(
      [this](const ui::Point& point) {
        corner_radius_.left_bottom = point;
        corner_radius_change_event_.Raise(corner_radius_);
      });

  right_bottom_editor_.PointChangeEvent()->AddHandler(
      [this](const ui::Point& point) {
        corner_radius_.right_bottom = point;
        corner_radius_change_event_.Raise(corner_radius_);
      });
}

CornerRadiusPropertyEditor::~CornerRadiusPropertyEditor() {
  container_.RemoveFromParent();
}

void CornerRadiusPropertyEditor::SetCornerRadius(
    const ui::CornerRadius& corner_radius) {
  left_top_editor_.SetPoint(corner_radius_.left_top);
  right_top_editor_.SetPoint(corner_radius_.right_top);
  left_bottom_editor_.SetPoint(corner_radius_.left_bottom);
  right_bottom_editor_.SetPoint(corner_radius_.right_bottom);
}
}  // namespace cru::theme_builder::components::properties
