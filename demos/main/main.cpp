#include <memory>
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/components/Menu.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/TextBox.h"
#include "cru/ui/controls/Window.h"
#include "cru/ui/events/UiEvents.h"
#include "cru/ui/host/WindowHost.h"

using cru::platform::gui::IUiApplication;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::TextBox;
using cru::ui::controls::Window;

int main() {
  IUiApplication* application = cru::platform::bootstrap::CreateUiApplication();

  auto application_menu = application->GetApplicationMenu();

  if (application_menu) {
    auto mi = application->GetApplicationMenu()
                  ->GetItemAt(0)
                  ->GetSubmenu()
                  ->CreateItemAt(0);
    mi->SetTitle(u"Quit");
    mi->SetKeyboardShortcut(cru::platform::gui::KeyCode::Q,
                            cru::platform::gui::KeyModifiers::command);
    mi->SetEnabled(true);
    mi->SetOnClickHandler([&]() { application->RequestQuit(0); });
  }

  const auto window = Window::Create();

  const auto flex_layout = FlexLayout::Create();
  flex_layout->SetFlexDirection(cru::ui::FlexDirection::Vertical);
  flex_layout->SetContentMainAlign(cru::ui::FlexCrossAlignment::Center);
  flex_layout->SetItemCrossAlign(cru::ui::FlexCrossAlignment::Center);

  window->AddChild(flex_layout, 0);

  const auto text_block = TextBlock::Create(u"Hello World from CruUI!", true);
  flex_layout->AddChild(text_block, 0);

  const auto button_text_block = TextBlock::Create(u"OK");
  const auto button = Button::Create();
  button->SetChild(button_text_block);
  flex_layout->AddChild(button, 1);

  const auto text_box = TextBox::Create();
  text_box->SetMultiLine(true);
  flex_layout->AddChild(text_box, 2);

  auto popup_menu = std::make_unique<cru::ui::components::PopupMenu>(window);
  popup_menu->GetMenu()->AddTextItem(u"Item 1", [] {});
  popup_menu->GetMenu()->AddTextItem(u"Item 2000", [] {});

  window->MouseDownEvent()->Bubble()->AddHandler(
      [window, &popup_menu](cru::ui::events::MouseButtonEventArgs& e) {
        if (e.GetButton() == cru::ui::mouse_buttons::right) {
          popup_menu->SetPosition(e.GetPoint() + window->GetWindowHost()
                                                     ->GetNativeWindow()
                                                     ->GetClientRect()
                                                     .GetLeftTop());
          popup_menu->Show();
        }
      });

  window->GetWindowHost()->GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);

  return application->Run();
}
