#pragma once
#include "../Editor.h"
#include "../LabeledMixin.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class PointPropertyEditor : public Editor, public LabeledMixin {
 public:
  using PropertyType = ui::Point;

  PointPropertyEditor();
  ~PointPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::Point GetValue() { return point_; }
  void SetValue(const ui::Point& point, bool trigger_change = true);

 private:
  static std::string ConvertPointToString(const ui::Point& point);

 private:
  ui::Point point_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBox text_;
  bool is_text_valid_;
};
}  // namespace cru::theme_builder::components::properties
