#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/components/PopupButton.h"
#include "cru/ui/components/Select.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"
#include "cru/ui/controls/Window.h"

using cru::platform::gui::IUiApplication;
using namespace cru::ui::controls;
using namespace cru::ui::components;

int main() {
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  auto application_menu = application->GetApplicationMenu();

  if (application_menu) {
    auto mi = application->GetApplicationMenu()
                  ->GetItemAt(0)
                  ->GetSubmenu()
                  ->CreateItemAt(0);
    mi->SetTitle("Quit");
    mi->SetKeyboardShortcut(cru::platform::gui::KeyCode::Q,
                            cru::platform::gui::KeyModifiers::Command);
    mi->SetEnabled(true);
    mi->SetOnClickHandler([&]() { application->RequestQuit(0); });
  }

  Window window;

  FlexLayout flex_layout;
  flex_layout.SetFlexDirection(FlexDirection::Vertical);
  flex_layout.SetContentMainAlign(FlexCrossAlignment::Center);
  flex_layout.SetItemCrossAlign(FlexCrossAlignment::Center);

  window.AddChild(&flex_layout);

  auto text_block = TextBlock::Create("Hello World from CruUI!", true);
  flex_layout.AddChild(text_block.get());

  auto button_text_block = TextBlock::Create("OK");
  Button button;
  button.SetChild(button_text_block.get());
  flex_layout.AddChild(&button);

  TextBox text_box;
  text_box.SetMultiLine(true);
  flex_layout.AddChild(&text_box);

  PopupMenuTextButton popup_menu_text_button;
  popup_menu_text_button.SetButtonText("Popup Menu Button");
  popup_menu_text_button.SetMenuItems({"Item 1", "Item 2", "Item 3"});
  flex_layout.AddChild(popup_menu_text_button.GetRootControl());

  Select select;
  select.SetItems({"Item 1", "Item 2", "Item 3"});
  flex_layout.AddChild(select.GetRootControl());

  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);

  return application->Run();
}
