#pragma once
#include "../Editor.h"
#include "cru/platform/graphics/Base.h"
#include "cru/ui/controls/Container.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"
#include "cru/ui/render/MeasureRequirement.h"

namespace cru::theme_builder::components::properties {
class MeasureLengthPropertyEditor : public Editor {
 public:
  using PropertyType = ui::render::MeasureLength;

  MeasureLengthPropertyEditor();
  ~MeasureLengthPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  std::string GetLabel() const { return label_.GetText(); }
  void SetLabel(std::string label) { label_.SetText(std::move(label)); }

  PropertyType GetValue() const { return measure_length_; }
  void SetValue(const PropertyType& value, bool trigger_change = true);

 private:
  PropertyType measure_length_;

  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox text_;
  bool is_text_valid_;
};
}  // namespace cru::theme_builder::components::properties
