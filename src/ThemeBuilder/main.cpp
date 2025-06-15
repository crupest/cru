#include "components/MainWindow.h"
#include <cru/Resource.h>
#include "cru/bootstrap/Bootstrap.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/ThemeResourceDictionary.h"

int main() {
  using namespace cru::theme_builder::components;
  using namespace cru::ui;

  auto resource_dir = cru::GetResourceDir();

  ThemeManager::GetInstance()->PrependThemeResourceDictionary(
      ThemeResourceDictionary::FromFile(
          resource_dir / "cru/theme_builder/ThemeResources.xml"));

  std::unique_ptr<cru::platform::gui::IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  auto main_window = std::make_unique<MainWindow>();

  main_window->Show();

  return application->Run();
}
