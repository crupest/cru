#include "cru/platform/bootstrap/Bootstrap.h"

#if defined(_WIN32)
#include "cru/platform/gui/win/UiApplication.h"
#elif defined(__APPLE__)
#include "cru/platform/gui/osx/UiApplication.h"
#elif defined(__unix)
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/gui/sdl/UiApplication.h"
#include "cru/platform/gui/xcb/UiApplication.h"
#else
#endif

namespace cru::platform::bootstrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#if defined(_WIN32)
  return new cru::platform::gui::win::WinUiApplication();
#elif defined(__APPLE__)
  return new cru::platform::gui::osx::OsxUiApplication();
#else
  // return new cru::platform::gui::xcb::XcbUiApplication();
  return new cru::platform::gui::sdl::SdlUiApplication(
      new cru::platform::graphics::cairo::CairoGraphicsFactory(), true);
#endif
  NotImplemented();
}
}  // namespace cru::platform::bootstrap
