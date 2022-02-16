#include "MainWindow.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/StackLayout.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::theme_builder {
using namespace cru::ui;
using namespace cru::ui::controls;
using namespace cru::platform::gui;

MainWindow::MainWindow() {
  window_.GetNativeWindow()->SetTitle(u"CruUI Theme Builder");
  main_layout_.SetFlexDirection(FlexDirection::Horizontal);
  window_.AddChild(&main_layout_);
  main_layout_.AddChild(&preview_layout_);

  preview_button_text_.SetText(u"Preview");
  preview_button_.SetChild(&preview_button_text_);
  preview_layout_.AddChild(&preview_button_);
  preview_layout_.SetChildLayoutData(
      0, StackChildLayoutData{Alignment::Center, Alignment::Center});

  style_rule_set_editor_.BindStyleRuleSet(
      preview_button_.GetStyleRuleSet()->GetParent());
  main_layout_.AddChild(style_rule_set_editor_.GetRootControl());
}

MainWindow::~MainWindow() {}

void MainWindow::Show() {
  window_.GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);
  window_.GetNativeWindow()->SetToForeground();
}
}  // namespace cru::theme_builder
