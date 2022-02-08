#include "StyleRuleSetEditor.h"

namespace cru::theme_builder {
StyleRuleSetEditor::StyleRuleSetEditor() {}

StyleRuleSetEditor::~StyleRuleSetEditor() {}

void StyleRuleSetEditor::BindStyleRuleSet(
    std::shared_ptr<ui::style::StyleRuleSet> rule_set) {
  style_rule_set_ = std::move(rule_set);
  UpdateView();
}

void StyleRuleSetEditor::UpdateView() {
  
}
}  // namespace cru::theme_builder
