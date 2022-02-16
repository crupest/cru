#pragma once
#include "StyleRuleEditor.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/ScrollView.h"
#include "cru/ui/style/StyleRuleSet.h"

namespace cru::theme_builder {
class StyleRuleSetEditor : public ui::components::Component {
 public:
  StyleRuleSetEditor();

  CRU_DELETE_COPY(StyleRuleSetEditor)
  CRU_DELETE_MOVE(StyleRuleSetEditor)

  ~StyleRuleSetEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &scroll_view_; }

  void BindStyleRuleSet(std::shared_ptr<ui::style::StyleRuleSet> rule_set);

 private:
  std::shared_ptr<ui::style::StyleRuleSet> style_rule_set_;

  ui::controls::ScrollView scroll_view_;
  ui::controls::FlexLayout container_;
  ui::controls::FlexLayout rules_layout_;
  std::vector<std::unique_ptr<StyleRuleEditor>> style_rule_editors_;
  ui::controls::Button add_button_;
  ui::controls::TextBlock add_button_text_;
};
}  // namespace cru::theme_builder
