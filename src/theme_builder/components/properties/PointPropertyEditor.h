#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class PointPropertyEditor : public ui::components::Component {
 public:
  PointPropertyEditor();
  ~PointPropertyEditor() override;

 public:
  ui::Point GetPoint() const { return point_; }
  void SetPoint(const ui::Point& point);

  IEvent<ui::Point>* PointChangeEvent() { return &point_change_event_; }

 private:
  static String ConvertPointToString(const ui::Point& point);

 private:
  ui::Point point_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox text_;
  bool is_text_valid_;

  Event<ui::Point> point_change_event_;
};
}  // namespace cru::theme_builder::components::properties
