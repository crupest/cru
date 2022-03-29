#include "HeadBodyEditor.h"
#include "Common.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder::components {
HeadBodyEditor::HeadBodyEditor() {
  border_.SetChild(&container_);
  border_.SetBackgroundBrush(CreateRandomEditorBackgroundBrush());

  container_.SetFlexDirection(ui::controls::FlexDirection::Vertical);
  container_.AddChild(&head_container_);
  container_.SetItemCrossAlign(ui::controls::FlexCrossAlignment::Stretch);
  head_container_.SetFlexDirection(ui::controls::FlexDirection::Horizontal);
  head_container_.AddChild(&label_);

  remove_button_.GetStyleRuleSet()->SetParent(
      ui::ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          u"cru.theme_builder.icon-button.style"));
  remove_button_.SetIconWithSvgPathDataStringResourceKey(u"icon.close",
                                                         {0, 0, 16, 16});
  remove_button_.SetIconFillColor(ui::colors::red);
  head_container_.AddChild(&remove_button_);

  remove_button_.ClickEvent()->AddSpyOnlyHandler(
      [this] { remove_event_.Raise(nullptr); });
}

HeadBodyEditor::~HeadBodyEditor() {}
}  // namespace cru::theme_builder::components
