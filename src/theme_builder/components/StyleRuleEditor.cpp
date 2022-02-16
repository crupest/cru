#include "StyleRuleEditor.h"
#include "conditions/ConditionEditor.h"
#include "cru/ui/style/StyleRule.h"

namespace cru::theme_builder {
StyleRuleEditor::StyleRuleEditor() {
  main_layout_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  main_layout_.AddChild(&remove_button_);

  remove_button_.SetChild(&remove_button_text_);
  remove_button_text_.SetText(u"X");

  main_layout_.AddChild(&right_layout_);
  right_layout_.SetFlexDirection(ui::controls::FlexDirection::Vertical);

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { remove_event_.Raise(nullptr); });
}

StyleRuleEditor::~StyleRuleEditor() {}

ui::style::StyleRule StyleRuleEditor::GetValue() const {
  return ui::style::StyleRule(condition_editor_->GetCondition(),
                              styler_editor_->GetStyler());
}

void StyleRuleEditor::SetValue(const ui::style::StyleRule& style_rule,
                               bool trigger_change) {
  right_layout_.ClearChildren();
  condition_editor_ =
      components::conditions::CreateConditionEditor(style_rule.GetCondition());
  styler_editor_ =
      components::stylers::CreateStylerEditor(style_rule.GetStyler());
  right_layout_.AddChild(condition_editor_->GetRootControl());
  right_layout_.AddChild(styler_editor_->GetRootControl());
}
}  // namespace cru::theme_builder
