#include "components/MainWindow.h"
#include "cru/common/io/Resource.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/ThemeResourceDictionary.h"

int main() {
  using namespace cru::theme_builder;
  using namespace cru::ui;

  auto resource_dir = cru::io::GetResourceDir();

  ThemeManager::GetInstance()->PrependThemeResourceDictionary(
      ThemeResourceDictionary::FromFile(resource_dir /
                                        "cru/theme_builder/ThemeResources.xml"));

  std::unique_ptr<cru::platform::gui::IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  auto main_window = std::make_unique<MainWindow>();

  main_window->Show();

  return application->Run();
}
