#include "CompoundStylerEditor.h"
#include "BorderStylerEditor.h"
#include "ContentBrushStylerEditor.h"
#include "CursorStylerEditor.h"
#include "FontStylerEditor.h"
#include "MarginStylerEditor.h"
#include "PaddingStylerEditor.h"
#include "PreferredSizeStylerEditor.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/DeleteLater.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/style/Styler.h"

namespace cru::theme_builder::components::stylers {
CompoundStylerEditor::CompoundStylerEditor() {
  SetLabel("Compound Styler");
  GetContainer()->AddChild(&children_container_);
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
  add_child_button_.SetMenuItems({"Compound Styler", "Border Styler",
                                  "Cursor Styler", "Content Brush Styler",
                                  "Font Styler", "Margin Styler",
                                  "Padding Styler", "Preferred Size Styler"});
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
      case 3:
        editor = std::make_unique<ContentBrushStylerEditor>();
        break;
      case 4:
        editor = std::make_unique<FontStylerEditor>();
        break;
      case 5:
        editor = std::make_unique<MarginStylerEditor>();
        break;
      case 6:
        editor = std::make_unique<PaddingStylerEditor>();
        break;
      case 7:
        editor = std::make_unique<PreferredSizeStylerEditor>();
        break;
      default:
        break;
    }
    if (editor) {
      ConnectChangeEvent(editor.get());
      editor->RemoveEvent()->AddSpyOnlyHandler([this, c = editor.get()] {
        auto index =
            this->children_container_.IndexOfChild(c->GetRootControl());
        this->children_.erase(this->children_.begin() + index);
        this->children_container_.RemoveChildAt(index);
        RaiseChangeEvent();
      });
      children_.push_back(ui::ToDeleteLaterPtr(std::move(editor)));
      children_container_.AddChild(editor->GetRootControl());
      RaiseChangeEvent();
    }
  });
}

CompoundStylerEditor::~CompoundStylerEditor() {}

ClonePtr<ui::style::CompoundStyler> CompoundStylerEditor::GetValue() {
  std::vector<ClonePtr<ui::style::Styler>> children_styler;
  for (auto& child : children_) {
    children_styler.push_back(child->GetStyler());
  }
  return ui::style::CompoundStyler::Create(std::move(children_styler));
}

void CompoundStylerEditor::SetValue(ui::style::CompoundStyler* value,
                                    bool trigger_change) {
  children_.clear();
  for (const auto& styler : value->GetChildren()) {
    auto editor = CreateStylerEditor(styler.get());
    ConnectChangeEvent(editor.get());
    editor->RemoveEvent()->AddSpyOnlyHandler([this, c = editor.get()] {
      auto index = this->children_container_.IndexOfChild(c->GetRootControl());
      this->children_.erase(this->children_.begin() + index);
      this->children_container_.RemoveChildAt(index);
      RaiseChangeEvent();
    });
    children_.push_back(ui::ToDeleteLaterPtr(std::move(editor)));
    children_container_.AddChild(children_.back()->GetRootControl());
  }
}
}  // namespace cru::theme_builder::components::stylers
