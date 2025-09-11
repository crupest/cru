#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/base/Base.h"

#if defined(_WIN32)
#include "cru/platform/gui/win/UiApplication.h"
#elif defined(__APPLE__)
#include "cru/platform/gui/osx/UiApplication.h"
#else
#endif

namespace cru::platform::bootstrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#if defined(_WIN32)
  return new cru::platform::gui::win::WinUiApplication();
#elif defined(__APPLE__)
  return new cru::platform::gui::osx::OsxUiApplication();
#else
  NotImplemented();
#endif
}
}  // namespace cru::platform::bootstrap
