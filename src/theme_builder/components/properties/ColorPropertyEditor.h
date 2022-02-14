#pragma once
#include "cru/platform/graphics/Base.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Container.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class ColorPropertyEditor : public ui::components::Component {
 public:
  using PropertyType = ui::Color;

  ColorPropertyEditor();
  ~ColorPropertyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  ui::Color GetValue() const { return color_; }
  void SetValue(const ui::Color& color, bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 private:
  ui::Color color_ = ui::colors::transparent;

  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::Container color_cube_;
  std::shared_ptr<platform::graphics::ISolidColorBrush> color_cube_brush_;
  ui::controls::TextBox color_text_;
  bool is_color_text_valid_;

  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components::properties
