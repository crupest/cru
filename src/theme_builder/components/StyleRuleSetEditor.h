#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/StyleRuleSet.h"

namespace cru::theme_builder {
class StyleRuleSetEditor : public ui::components::Component {
 public:
  StyleRuleSetEditor();

  CRU_DELETE_COPY(StyleRuleSetEditor)
  CRU_DELETE_MOVE(StyleRuleSetEditor)

  ~StyleRuleSetEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &main_layout_; }

  void BindStyleRuleSet(std::shared_ptr<ui::style::StyleRuleSet> rule_set);

 private:
  void UpdateView();

 private:
  ui::controls::FlexLayout main_layout_;

  std::shared_ptr<ui::style::StyleRuleSet> style_rule_set_;
};
}  // namespace cru::theme_builder
