#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

using cru::platform::gui::INativeWindow;
using cru::platform::gui::IUiApplication;
using cru::platform::gui::WindowVisibilityType;

int main() {
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  application->SetQuitOnAllWindowClosed(true);

  std::unique_ptr<INativeWindow> window1(application->CreateWindow());
  window1->SetVisibility(WindowVisibilityType::Show);
  window1->SetToForeground();

  std::unique_ptr<INativeWindow> window2(application->CreateWindow());
  window2->SetVisibility(WindowVisibilityType::Show);
  window2->SetToForeground();

  return application->Run();
}
