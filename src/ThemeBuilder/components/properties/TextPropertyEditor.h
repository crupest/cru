#pragma once
#include "../LabeledMixin.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class TextPropertyEditor : public ui::components::Component,
                           public LabeledMixin {
 public:
  TextPropertyEditor();
  ~TextPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  std::string GetText() { return editor_.GetText(); }
  std::string_view GetTextView() { return editor_.GetTextView(); }
  void SetText(std::string text) { editor_.SetText(std::move(text)); }

 protected:
  virtual bool Validate(std::string_view text, std::string* error_message);
  virtual void OnTextChanged(std::string_view text);

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox editor_;
};
}  // namespace cru::theme_builder::components::properties
