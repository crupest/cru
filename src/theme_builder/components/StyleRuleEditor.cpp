#include "StyleRuleEditor.h"

namespace cru::theme_builder {
StyleRuleEditor::StyleRuleEditor() {}

StyleRuleEditor::~StyleRuleEditor() { main_layout_.RemoveFromParent(); }

void StyleRuleEditor::BindStyleRule(ui::style::StyleRule *rule) {
  style_rule_ = rule;
  UpdateView();
}

void StyleRuleEditor::UpdateView() {}

}  // namespace cru::theme_builder
