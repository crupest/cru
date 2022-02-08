#include "components/MainWindow.h"
#include "cru/platform/bootstrap/Bootstrap.h"

int main() {
  using namespace cru::theme_builder;

  std::unique_ptr<cru::platform::gui::IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  auto main_window = std::make_unique<MainWindow>();

  main_window->Show();

  return application->Run();
}
