#include "StyleRuleEditor.h"

namespace cru::theme_builder {
StyleRuleEditor::StyleRuleEditor() {
  main_layout_ = ui::controls::FlexLayout::Create();
}

StyleRuleEditor::~StyleRuleEditor() {
  main_layout_->RemoveFromParent();
  delete main_layout_;
}

void StyleRuleEditor::BindStyleRule(ui::style::StyleRule *rule) {
  style_rule_ = rule;
  UpdateView();
}

void StyleRuleEditor::UpdateView() {}

}  // namespace cru::theme_builder
