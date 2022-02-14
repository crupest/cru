#pragma once
#include "PointPropertyEditor.h"
#include "cru/ui/Base.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components::properties {
class CornerRadiusPropertyEditor : public ui::components::Component {
 public:
  using PropertyType = ui::CornerRadius;

  CornerRadiusPropertyEditor();
  ~CornerRadiusPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::CornerRadius GetValue() const { return corner_radius_; }
  void SetValue(const ui::CornerRadius& corner_radius,
                bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  ui::CornerRadius corner_radius_;

  ui::controls::FlexLayout container_;
  PointPropertyEditor left_top_editor_;
  PointPropertyEditor right_top_editor_;
  PointPropertyEditor left_bottom_editor_;
  PointPropertyEditor right_bottom_editor_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::properties
