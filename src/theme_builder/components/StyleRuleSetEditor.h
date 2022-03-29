#pragma once
#include "StyleRuleEditor.h"
#include "cru/ui/DeleteLater.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/ScrollView.h"
#include "cru/ui/model/IListChangeNotify.h"
#include "cru/ui/style/StyleRuleSet.h"

namespace cru::theme_builder::components {
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
  Index IndexOfRuleEditor(StyleRuleEditor* editor);

  void UpdateView(ui::style::StyleRuleSet* style_rule_set,
                  std::optional<ui::model::ListChange> change = std::nullopt);

 private:
  std::shared_ptr<ui::style::StyleRuleSet> style_rule_set_;

  ui::controls::ScrollView scroll_view_;
  ui::controls::FlexLayout container_;
  ui::controls::FlexLayout rules_layout_;
  std::vector<ui::DeleteLaterPtr<StyleRuleEditor>> style_rule_editors_;
  ui::controls::IconButton add_button_;

  bool suppress_next_set_ = false;
};
}  // namespace cru::theme_builder::components
