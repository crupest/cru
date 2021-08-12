#include "cru/platform/bootstrap/Bootstrap.hpp"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/win/gui/UiApplication.hpp"
#else
#endif

namespace cru::platform::boostrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::platform::gui::win::WinUiApplication();
#else
#endif
}
}  // namespace cru::platform::boostrap
