#include "CompoundStylerEditor.h"
#include "BorderStylerEditor.h"
#include "CursorStylerEditor.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
CompoundStylerEditorChild::CompoundStylerEditorChild(
    std::unique_ptr<StylerEditor>&& editor)
    : styler_editor_(std::move(editor)) {
  container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  container_.AddChild(&remove_button_);

  remove_button_.SetChild(&remove_button_text_);
  remove_button_text_.SetText(u"X");

  container_.AddChild(styler_editor_->GetRootControl());

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { this->remove_event_.Raise(nullptr); });
}

CompoundStylerEditorChild::~CompoundStylerEditorChild() {}

CompoundStylerEditor::CompoundStylerEditor() {
  SetLabel(u"Compound Styler");
  children_container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  GetContainer()->AddChild(&children_container_);

  add_child_button_.SetButtonText(u"+");
  add_child_button_.SetMenuItems({
      u"Compound Styler",
      u"Border Styler",
      u"Cursor Styler",
  });
  add_child_button_.MenuItemSelectedEvent()->AddHandler([this](Index index) {
    std::unique_ptr<StylerEditor> editor;
    switch (index) {
      case 0:
        editor = std::make_unique<CompoundStylerEditor>();
        break;
      case 1:
        editor = std::make_unique<BorderStylerEditor>();
        break;
      case 2:
        editor = std::make_unique<CursorStylerEditor>();
        break;
      default:
        break;
    }
    if (editor) {
      ConnectChangeEvent(editor.get());
      auto child =
          std::make_unique<CompoundStylerEditorChild>(std::move(editor));
      child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
        auto index = this->children_container_.IndexOf(c->GetRootControl());
        this->children_.erase(this->children_.begin() + index);
        this->children_container_.RemoveChildAt(index);
        RaiseChangeEvent();
      });
      children_container_.AddChild(child->GetRootControl());
      children_.push_back(std::move(child));
      RaiseChangeEvent();
    }
  });
}

CompoundStylerEditor::~CompoundStylerEditor() {}

ClonablePtr<ui::style::CompoundStyler> CompoundStylerEditor::GetValue() {
  std::vector<ClonablePtr<ui::style::Styler>> children_styler;
  for (auto& child : children_) {
    children_styler.push_back(child->GetStylerEditor()->GetStyler());
  }
  return ui::style::CompoundStyler::Create(std::move(children_styler));
}

void CompoundStylerEditor::SetValue(ui::style::CompoundStyler* value,
                                    bool trigger_change) {
  children_.clear();
  children_container_.ClearChildren();
  for (const auto& styler : value->GetChildren()) {
    auto editor = CreateStylerEditor(styler.get());
    ConnectChangeEvent(editor.get());
    auto child = std::make_unique<CompoundStylerEditorChild>(std::move(editor));
    child->RemoveEvent()->AddSpyOnlyHandler([this, c = child.get()] {
      auto index = this->children_container_.IndexOf(c->GetRootControl());
      this->children_.erase(this->children_.begin() + index);
      this->children_container_.RemoveChildAt(index);
      RaiseChangeEvent();
    });
    children_.push_back(std::move(child));
    children_container_.AddChild(children_.back()->GetRootControl());
  }
}
}  // namespace cru::theme_builder::components::stylers
