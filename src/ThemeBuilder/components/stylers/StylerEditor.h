#pragma once
#include "../HeadBodyEditor.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
class StylerEditor : public HeadBodyEditor {
 public:
  StylerEditor();
  ~StylerEditor() override;

 public:
  virtual ClonablePtr<ui::style::Styler> GetStyler() = 0;
};

std::unique_ptr<StylerEditor> CreateStylerEditor(ui::style::Styler* styler);
}  // namespace cru::theme_builder::components::stylers
