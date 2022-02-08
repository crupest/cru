#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/StyleRule.h"

namespace cru::theme_builder {
class StyleRuleEditor : public ui::components::Component {
 public:
  StyleRuleEditor();

  CRU_DELETE_COPY(StyleRuleEditor)
  CRU_DELETE_MOVE(StyleRuleEditor)

  ~StyleRuleEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return main_layout_; }

  void BindStyleRule(ui::style::StyleRule* rule);

 private:
  void UpdateView();

 private:
  ui::controls::FlexLayout* main_layout_;

  ui::style::StyleRule* style_rule_;
};
}  // namespace cru::theme_builder
