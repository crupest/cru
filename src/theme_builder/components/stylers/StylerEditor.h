#pragma once
#include "../Editor.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class StylerEditor : public Editor {
 public:
  StylerEditor();
  ~StylerEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::controls::FlexLayout* GetContainer() { return &container_; }

  ui::controls::FlexLayout* GetHeadContainer() { return &head_container_; }

  String GetLabel() const { return label_.GetText(); }
  void SetLabel(String label) { label_.SetText(std::move(label)); }

  virtual ClonablePtr<ui::style::Styler> GetStyler() = 0;

 private:
  ui::controls::FlexLayout container_;
  ui::controls::FlexLayout head_container_;
  ui::controls::TextBlock label_;
};

std::unique_ptr<StylerEditor> CreateStylerEditor(ui::style::Styler* styler);
}  // namespace cru::theme_builder::components::stylers
