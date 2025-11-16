#pragma once
#include "StylerEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/DeleteLater.h"
#include "cru/ui/components/PopupButton.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class CompoundStylerEditor : public StylerEditor {
 public:
  CompoundStylerEditor();
  ~CompoundStylerEditor() override;

 public:
  ClonePtr<ui::style::CompoundStyler> GetValue();
  void SetValue(ui::style::CompoundStyler* styler, bool trigger_change = true);
  void SetValue(const ClonePtr<ui::style::CompoundStyler>& styler,
                bool trigger_change = true) {
    SetValue(styler.get(), trigger_change);
  }

  ClonePtr<ui::style::Styler> GetStyler() override { return GetValue(); }

 private:
  ui::controls::FlexLayout children_container_;
  std::vector<ui::DeleteLaterPtr<StylerEditor>> children_;
  ui::components::PopupMenuIconButton add_child_button_;
};
}  // namespace cru::theme_builder::components::stylers
