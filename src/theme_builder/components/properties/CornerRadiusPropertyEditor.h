#pragma once
#include "PointPropertyEditor.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
class CornerRadiusPropertyEditor : public ui::components::Component {
 public:
  CornerRadiusPropertyEditor();
  ~CornerRadiusPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::CornerRadius GetCornerRadius() const { return corner_radius_; }
  void SetCornerRadius(const ui::CornerRadius& corner_radius);

  IEvent<ui::CornerRadius>* CornerRadiusChangeEvent() {
    return &corner_radius_change_event_;
  }

 private:
  ui::CornerRadius corner_radius_;

  ui::controls::FlexLayout container_;
  PointPropertyEditor left_top_editor_;
  PointPropertyEditor right_top_editor_;
  PointPropertyEditor left_bottom_editor_;
  PointPropertyEditor right_bottom_editor_;

  Event<ui::CornerRadius> corner_radius_change_event_;
};
}  // namespace cru::theme_builder::components::properties
