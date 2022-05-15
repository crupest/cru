#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"

namespace cru::theme_builder::components::properties {
class TextPropertyEditor : public ui::components::Component {
 public:
  TextPropertyEditor();
  ~TextPropertyEditor() override;

  ui::controls::Control* GetRootControl() override { return &container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  String GetText() const { return editor_.GetText(); }
  StringView GetTextView() const { return editor_.GetTextView(); }
  void SetText(String text) { editor_.SetText(std::move(text)); }

 protected:
  virtual bool Validate(StringView text, String* error_message);
  virtual void OnTextChanged(StringView text);

 private:
  ui::controls::FlexLayout container_;
  ui::controls::TextBlock label_;
  ui::controls::TextBox editor_;
};
}  // namespace cru::theme_builder::components::properties
