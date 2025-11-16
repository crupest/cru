#include "CompoundConditionEditor.h"
#include "CheckedConditionEditor.h"
#include "ClickStateConditionEditor.h"
#include "ConditionEditor.h"
#include "FocusConditionEditor.h"
#include "NoConditionEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/platform/Color.h"
#include "cru/ui/Base.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {

CompoundConditionEditor::CompoundConditionEditor() {
  SetLabel("Compound Condition");

  GetContainer()->AddChild(&children_container_);
  children_container_.SetMargin({10, 0, 0, 0});
  children_container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  children_container_.SetItemCrossAlign(
      ui::controls::FlexCrossAlignment::Start);

  GetHeadContainer()->AddChild(add_child_button_.GetRootControl());

  add_child_button_.GetButton()->GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          "cru.theme_builder.icon-button.style"));
  add_child_button_.GetButton()->SetIconWithSvgPathDataStringResourceKey(
      "icon.plus", {0, 0, 16, 16});
  add_child_button_.GetButton()->SetPreferredSize({24, 24});
  add_child_button_.GetButton()->SetPadding(ui::Thickness(2));
  add_child_button_.GetButton()->SetIconFillColor(ui::colors::green);
  add_child_button_.SetMenuItems({"And Condition", "Or Condition",
                                  "Click State Condition", "Focus Condition",
                                  "Checked Condition", "No Condition"});
  add_child_button_.MenuItemSelectedEvent()->AddHandler([this](Index index) {
    std::unique_ptr<ConditionEditor> editor;
    switch (index) {
      case 0:
        editor = std::make_unique<AndConditionEditor>();
        break;
      case 1:
        editor = std::make_unique<OrConditionEditor>();
        break;
      case 2:
        editor = std::make_unique<ClickStateConditionEditor>();
        break;
      case 3:
        editor = std::make_unique<FocusConditionEditor>();
        break;
      case 4:
        editor = std::make_unique<CheckedConditionEditor>();
        break;
      case 5:
        editor = std::make_unique<NoConditionEditor>();
        break;
      default:
        break;
    }
    if (editor) {
      ConnectChangeEvent(editor.get());
      editor->RemoveEvent()->AddSpyOnlyHandler([this, c = editor.get()] {
        auto index = this->children_container_.IndexOf(c->GetRootControl());
        this->children_.erase(this->children_.begin() + index);
        this->children_container_.RemoveChildAt(index);
        RaiseChangeEvent();
      });
      children_.push_back(std::move(editor));
      children_container_.AddChild(children_.back()->GetRootControl());
      RaiseChangeEvent();
    }
  });
}

CompoundConditionEditor::~CompoundConditionEditor() {}

std::vector<ClonePtr<ui::style::Condition>>
CompoundConditionEditor::GetChildren() {
  std::vector<ClonePtr<ui::style::Condition>> children;
  for (auto& child : children_) {
    children.push_back(child->GetCondition());
  }
  return children;
}

void CompoundConditionEditor::SetChildren(
    std::vector<ClonePtr<ui::style::Condition>> children,
    bool trigger_change) {
  children_container_.ClearChildren();
  children_.clear();
  for (const auto& condition : children) {
    auto editor = CreateConditionEditor(condition.get());
    ConnectChangeEvent(editor.get());
    editor->RemoveEvent()->AddSpyOnlyHandler([this, c = editor.get()] {
      auto index = this->children_container_.IndexOf(c->GetRootControl());
      this->children_.erase(this->children_.begin() + index);
      this->children_container_.RemoveChildAt(index);
      RaiseChangeEvent();
    });
    children_.push_back(std::move(editor));
    children_container_.AddChild(children_.back()->GetRootControl());
  }
  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::conditions
