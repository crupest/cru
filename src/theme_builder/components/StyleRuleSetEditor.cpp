#include "StyleRuleSetEditor.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder {
using namespace cru::ui::controls;
StyleRuleSetEditor::StyleRuleSetEditor() {}

StyleRuleSetEditor::~StyleRuleSetEditor() { main_layout_.RemoveFromParent(); }

void StyleRuleSetEditor::BindStyleRuleSet(
    std::shared_ptr<ui::style::StyleRuleSet> rule_set) {
  style_rule_set_ = std::move(rule_set);
  UpdateView();
}

void StyleRuleSetEditor::UpdateView() {}
}  // namespace cru::theme_builder
