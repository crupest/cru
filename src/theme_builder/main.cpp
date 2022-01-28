#include "cru/platform/bootstrap/Bootstrap.hpp"
#include "cru/platform/gui/UiApplication.hpp"

int main() {
  std::unique_ptr<cru::platform::gui::IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  return application->Run();
}
