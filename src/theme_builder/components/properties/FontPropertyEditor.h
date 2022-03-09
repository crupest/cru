#pragma once
#include "../Editor.h"
#include "cru/platform/graphics/Font.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class FontPropertyEditor : public Editor {
 public:
  using PropertyType = std::shared_ptr<platform::graphics::IFont>;

  FontPropertyEditor();
  ~FontPropertyEditor() override;

  ui::controls::Control* GetRootControl() override;

  String GetLabelText() const { return label_.GetText(); }
  void SetLabelText(String label) { label_.SetText(std::move(label)); }

  std::shared_ptr<platform::graphics::IFont> GetValue() const;
  void SetValue(std::shared_ptr<platform::graphics::IFont> value,
                bool trigger_change = true);

 private:
  ui::controls::FlexLayout main_container_;
  ui::controls::TextBlock label_;
  ui::controls::FlexLayout right_container_;
  ui::controls::FlexLayout font_family_container_;
  ui::controls::TextBlock font_family_label_;
  ui::controls::TextBox font_family_text_;
  ui::controls::FlexLayout font_size_container_;
  ui::controls::TextBlock font_size_label_;
  ui::controls::TextBox font_size_text_;
};
}  // namespace cru::theme_builder::components::properties
