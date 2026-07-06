#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/controls/Window.h"
#include "cru/ui/helper/ClickDetector.h"

#include <format>
#include <memory>

using cru::platform::gui::IUiApplication;
using cru::ui::controls::FlexCrossAlignment;
using cru::ui::controls::FlexDirection;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::Window;
using cru::ui::helper::ClickDetector;

int main() {
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  Window window;
  ClickDetector click_detector(&window);

  FlexLayout layout;
  layout.SetFlexDirection(FlexDirection::Vertical);
  layout.SetContentMainAlign(FlexCrossAlignment::Center);
  layout.SetItemCrossAlign(FlexCrossAlignment::Center);
  window.AddChild(&layout);

  auto message_text = TextBlock::Create("Clicked 0 times");
  layout.AddChild(message_text.get());

  click_detector.ClickEvent()->AddHandler(
      [&message_text](const cru::ui::helper::ClickEventArgs& args) {
        message_text->SetText(
            std::format("Clicked {} times", args.GetClickCount()));
      });

  window.GetNativeWindow()->SetVisibility(
      cru::platform::gui::WindowVisibilityType::Show);

  application->SetQuitOnAllWindowClosed(true);

  return application->Run();
}
