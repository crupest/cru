#include "StyleRuleEditor.h"
#include "Common.h"
#include "conditions/ConditionEditor.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/style/StyleRule.h"

namespace cru::theme_builder::components {
StyleRuleEditor::StyleRuleEditor() {
  container_.SetChild(&main_layout_);
  container_.SetBackgroundBrush(CreateRandomEditorBackgroundBrush());

  main_layout_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  main_layout_.SetItemCrossAlign(ui::controls::FlexCrossAlignment::Start);

  main_layout_.AddChild(&head_layout_);

  label_.SetText("Style Rule");
  head_layout_.AddChild(&label_);
  head_layout_.AddChild(&remove_button_);

  remove_button_.GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          "cru.theme_builder.icon-button.style"));
  remove_button_.SetIconWithSvgPathDataStringResourceKey("icon.close",
                                                         {0, 0, 16, 16});
  remove_button_.SetIconFillColor(ui::colors::red);

  main_layout_.AddChild(&body_layout_);
  body_layout_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  body_layout_.SetItemCrossAlign(ui::controls::FlexCrossAlignment::Start);
  body_layout_.SetMargin({10, 0, 0, 0});

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
  body_layout_.ClearChildren();
  condition_editor_ =
      components::conditions::CreateConditionEditor(style_rule.GetCondition());
  styler_editor_ =
      components::stylers::CreateStylerEditor(style_rule.GetStyler());
  body_layout_.AddChild(condition_editor_->GetRootControl());
  body_layout_.AddChild(styler_editor_->GetRootControl());
  condition_editor_->ChangeEvent()->AddSpyOnlyHandler(
      [this] { change_event_.Raise(nullptr); });
  styler_editor_->ChangeEvent()->AddSpyOnlyHandler(
      [this] { change_event_.Raise(nullptr); });

  if (trigger_change) {
    change_event_.Raise(nullptr);
  }
}
}  // namespace cru::theme_builder::components
