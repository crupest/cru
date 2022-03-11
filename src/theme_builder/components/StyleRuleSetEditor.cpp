#include "StyleRuleSetEditor.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/render/FlexLayoutRenderObject.h"
#include "cru/ui/style/Condition.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components {
using namespace cru::ui::controls;
StyleRuleSetEditor::StyleRuleSetEditor() {
  scroll_view_.SetChild(&container_);

  container_.SetFlexDirection(ui::render::FlexDirection::Vertical);
  container_.AddChild(&rules_layout_);
  container_.AddChild(&add_button_);

  rules_layout_.SetFlexDirection(ui::controls::FlexDirection::Vertical);

  add_button_.GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          u"cru.theme_builder.icon-button.style"));
  add_button_.SetIconWithSvgPathDataStringResourceKey(u"icon.plus",
                                                      {0, 0, 16, 16});
  add_button_.SetPreferredSize({24, 24});
  add_button_.SetPadding(ui::Thickness(2));
  add_button_.SetIconFillColor(ui::colors::green);

  add_button_.ClickEvent()->AddSpyOnlyHandler([this] {
    auto rule_set = ui::style::StyleRule(ui::style::NoCondition::Create(),
                                         ui::style::CompoundStyler::Create({}));
    style_rule_set_->AddStyleRule(rule_set);
  });
}

StyleRuleSetEditor::~StyleRuleSetEditor() {}

void StyleRuleSetEditor::BindStyleRuleSet(
    std::shared_ptr<ui::style::StyleRuleSet> rule_set) {
  Expects(style_rule_set_ == nullptr && rule_set);
  style_rule_set_ = std::move(rule_set);
  UpdateView(style_rule_set_.get());
  style_rule_set_->ChangeEvent()->AddSpyOnlyHandler(
      [this] { UpdateView(style_rule_set_.get()); });
}

void StyleRuleSetEditor::UpdateView(ui::style::StyleRuleSet* style_rule_set) {
  style_rule_editors_.clear();

  for (Index i = 0; i < style_rule_set->GetSize(); ++i) {
    const auto& rule = style_rule_set->GetStyleRule(i);
    auto style_rule_editor = std::make_unique<StyleRuleEditor>();
    style_rule_editor->SetValue(rule, false);
    style_rule_editor->RemoveEvent()->AddSpyOnlyHandler(
        [this, i] { style_rule_set_->RemoveStyleRule(i); });
    style_rule_editor->ChangeEvent()->AddSpyOnlyHandler(
        [this, i, editor = style_rule_editor.get()]() {
          style_rule_set_->SetStyleRule(i, editor->GetValue());
        });
    style_rule_editors_.push_back(std::move(style_rule_editor));
    rules_layout_.AddChild(style_rule_editors_.back()->GetRootControl());
  }
}
}  // namespace cru::theme_builder::components
