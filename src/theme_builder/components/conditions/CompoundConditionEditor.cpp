#include "CompoundConditionEditor.h"
#include "CheckedConditionEditor.h"
#include "ClickStateConditionEditor.h"
#include "ConditionEditor.h"
#include "FocusConditionEditor.h"
#include "NoConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/platform/Color.h"
#include "cru/ui/Base.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
CompoundConditionEditorChild::CompoundConditionEditorChild(
    std::unique_ptr<ConditionEditor>&& condition_editor)
    : condition_editor_(std::move(condition_editor)) {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&remove_button_);

  remove_button_.GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          u"cru.theme_builder.icon-button.style"));
  remove_button_.SetIconWithSvgPathDataStringResourceKey(u"icon.close",
                                                         {0, 0, 16, 16});
  remove_button_.SetPreferredSize({24, 24});
  remove_button_.SetPadding(ui::Thickness(2));
  remove_button_.SetIconFillColor(ui::colors::red);

  container_.AddChild(condition_editor_->GetRootControl());

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { this->remove_event_.Raise(nullptr); });
}

CompoundConditionEditorChild::~CompoundConditionEditorChild() {}

CompoundConditionEditor::CompoundConditionEditor() {
  SetLabel(u"Compound Condition");

  GetContainer()->AddChild(&children_container_);
  children_container_.SetItemCrossAlign(
      ui::controls::FlexCrossAlignment::Start);

  GetHeadContainer()->AddChild(add_child_button_.GetRootControl());

  add_child_button_.GetButton()->GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          u"cru.theme_builder.icon-button.style"));
  add_child_button_.GetButton()->SetIconWithSvgPathDataStringResourceKey(
      u"icon.plus-square", {0, 0, 16, 16});
  add_child_button_.GetButton()->SetPreferredSize({24, 24});
  add_child_button_.GetButton()->SetPadding(ui::Thickness(2));
  add_child_button_.GetButton()->SetIconFillColor(ui::colors::green);
  add_child_button_.SetMenuItems({u"And Condition", u"Or Condition",
                                  u"Click State Condition", u"Focus Condition",
                                  u"Checked Condition", u"No Condition"});
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
      auto child =
          std::make_unique<CompoundConditionEditorChild>(std::move(editor));
      child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
        auto index = this->children_container_.IndexOf(c->GetRootControl());
        this->children_.erase(this->children_.begin() + index);
        this->children_container_.RemoveChildAt(index);
        RaiseChangeEvent();
      });
      children_.push_back(std::move(child));
      children_container_.AddChild(children_.back()->GetRootControl());
      RaiseChangeEvent();
    }
  });
}

CompoundConditionEditor::~CompoundConditionEditor() {}

std::vector<ClonablePtr<ui::style::Condition>>
CompoundConditionEditor::GetChildren() {
  std::vector<ClonablePtr<ui::style::Condition>> children;
  for (auto& child : children_) {
    children.push_back(child->GetConditionEditor()->GetCondition());
  }
  return children;
}

void CompoundConditionEditor::SetChildren(
    std::vector<ClonablePtr<ui::style::Condition>> children,
    bool trigger_change) {
  children_container_.ClearChildren();
  children_.clear();
  for (const auto& condition : children) {
    auto condition_editor = CreateConditionEditor(condition.get());
    ConnectChangeEvent(condition_editor.get());
    auto child = std::make_unique<CompoundConditionEditorChild>(
        std::move(condition_editor));
    child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
      auto index = this->children_container_.IndexOf(c->GetRootControl());
      this->children_.erase(this->children_.begin() + index);
      this->children_container_.RemoveChildAt(index);
      RaiseChangeEvent();
    });
    children_.push_back(std::move(child));
    children_container_.AddChild(children_.back()->GetRootControl());
  }
  if (trigger_change) {
    RaiseChangeEvent();
  }
}
}  // namespace cru::theme_builder::components::conditions
