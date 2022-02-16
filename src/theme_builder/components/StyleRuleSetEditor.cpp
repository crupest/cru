#include "StyleRuleSetEditor.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Condition.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder {
using namespace cru::ui::controls;
StyleRuleSetEditor::StyleRuleSetEditor() {
  container_.AddChild(&rules_layout_);
  container_.AddChild(&add_button_);

  add_button_.SetChild(&add_button_text_);
  add_button_text_.SetText(u"+");

  add_button_.ClickEvent()->AddSpyOnlyHandler([this] {
    auto rule_set = ui::style::StyleRule(ui::style::AndCondition::Create({}),
                                         ui::style::CompoundStyler::Create({}));
    style_rule_set_->AddStyleRule(rule_set);
    style_rule_editors_.push_back(std::make_unique<StyleRuleEditor>());
    style_rule_editors_.back()->SetValue(rule_set);
    rules_layout_.AddChild(style_rule_editors_.back()->GetRootControl());
  });
}

StyleRuleSetEditor::~StyleRuleSetEditor() { rules_layout_.RemoveFromParent(); }

void StyleRuleSetEditor::BindStyleRuleSet(
    std::shared_ptr<ui::style::StyleRuleSet> rule_set) {
  style_rule_set_ = std::move(rule_set);

  rules_layout_.ClearChildren();
  style_rule_editors_.clear();

  for (Index i = 0; i < style_rule_set_->GetSize(); ++i) {
    const auto& rule = style_rule_set_->GetStyleRule(i);
    auto style_rule_editor = std::make_unique<StyleRuleEditor>();
    style_rule_editor->SetValue(rule, false);
    style_rule_editor->RemoveEvent()->AddSpyOnlyHandler([this, i] {
      style_rule_set_->RemoveStyleRule(i);
      style_rule_editors_.erase(style_rule_editors_.begin() + i);
    });
    style_rule_editor->ChangeEvent()->AddSpyOnlyHandler(
        [this, i, editor = style_rule_editor.get()]() {
          style_rule_set_->SetStyleRule(i, editor->GetValue());
        });
    style_rule_editors_.push_back(std::move(style_rule_editor));
    rules_layout_.AddChild(style_rule_editors_.back()->GetRootControl());
  }
}
}  // namespace cru::theme_builder
