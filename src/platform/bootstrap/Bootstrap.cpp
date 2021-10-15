#include "cru/platform/bootstrap/Bootstrap.hpp"
#include "cru/osx/gui/UiApplication.hpp"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/win/gui/UiApplication.hpp"
#else
#endif

namespace cru::platform::bootstrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::platform::gui::win::WinUiApplication();
#elif CRU_PLATFORM_OSX
  return new cru::platform::gui::osx::OsxUiApplication();
#endif
}
}  // namespace cru::platform::bootstrap
