#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

using cru::platform::gui::IUiApplication;
using cru::platform::gui::WindowVisibilityType;

int main() {
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  application->SetQuitOnAllWindowClosed(true);
  auto window = application->CreateWindow();
  window->SetVisibility(WindowVisibilityType::Show);
  window->SetToForeground();

  return application->Run();
}
