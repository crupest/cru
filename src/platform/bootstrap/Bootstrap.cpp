#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/osx/gui/UiApplication.h"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/win/gui/UiApplication.h"
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
