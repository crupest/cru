#pragma once
#include "StyleRuleSetEditor.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/StackLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/Window.h"

namespace cru::theme_builder::components {
class MainWindow : public ui::components::Component {
 public:
  MainWindow();

  CRU_DELETE_COPY(MainWindow)
  CRU_DELETE_MOVE(MainWindow)

  ~MainWindow() override;

  ui::controls::Control* GetRootControl() override { return &window_; }

  void Show();

 private:
  ui::controls::Window window_;
  ui::controls::FlexLayout main_layout_;
  ui::controls::StackLayout preview_layout_;
  ui::controls::Button preview_button_;
  ui::controls::TextBlock preview_button_text_;
  StyleRuleSetEditor style_rule_set_editor_;
};
}  // namespace cru::theme_builder::components
