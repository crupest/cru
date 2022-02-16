#include "CompoundConditionEditor.h"
#include "CheckedConditionEditor.h"
#include "ClickStateConditionEditor.h"
#include "ConditionEditor.h"
#include "FocusConditionEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Condition.h"

namespace cru::theme_builder::components::conditions {
CompoundConditionEditorChild::CompoundConditionEditorChild(
    std::unique_ptr<ConditionEditor>&& condition_editor)
    : condition_editor_(std::move(condition_editor)) {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&remove_button_);

  remove_button_.SetChild(&remove_button_text_);
  remove_button_text_.SetText(u"-");

  container_.AddChild(condition_editor_->GetRootControl());

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { this->remove_event_.Raise(nullptr); });
}

CompoundConditionEditorChild::~CompoundConditionEditorChild() {
  container_.RemoveFromParent();
}

CompoundConditionEditor::CompoundConditionEditor() {
  GetContainer()->AddChild(&children_container_);
  GetContainer()->AddChild(add_child_button_.GetRootControl());
  add_child_button_.SetButtonText(u"+");
  add_child_button_.SetMenuItems({
      u"And Condtion",
      u"Or Condtion",
      u"Click State Condtion",
      u"Focus Condtion",
      u"Checked Condtion",
  });
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
      default:
        break;
    }
    if (editor) {
      auto child =
          std::make_unique<CompoundConditionEditorChild>(std::move(editor));
      child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
        auto index = this->children_container_.IndexOf(c->GetRootControl());
        this->children_.erase(this->children_.begin() + index);
        this->children_container_.RemoveChildAt(index);
        change_event_.Raise(nullptr);
      });
      children_.push_back(std::move(child));
      children_container_.AddChild(children_.back()->GetRootControl());
      change_event_.Raise(nullptr);
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
    auto child = std::make_unique<CompoundConditionEditorChild>(
        std::move(condition_editor));
    child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
      auto index = this->children_container_.IndexOf(c->GetRootControl());
      this->children_.erase(this->children_.begin() + index);
      this->children_container_.RemoveChildAt(index);
      change_event_.Raise(nullptr);
    });
    children_.push_back(std::move(child));
    children_container_.AddChild(children_.back()->GetRootControl());
  }
  if (trigger_change) {
    change_event_.Raise(nullptr);
  }
}
}  // namespace cru::theme_builder::components::conditions
