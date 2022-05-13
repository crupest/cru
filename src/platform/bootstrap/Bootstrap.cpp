#include "cru/platform/bootstrap/Bootstrap.h"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/win/graphics/direct/Factory.h"
#include "cru/win/gui/UiApplication.h"
#else
#include "cru/osx/graphics/quartz/Factory.h"
#include "cru/osx/gui/UiApplication.h"
#endif

namespace cru::platform::bootstrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::platform::gui::win::WinUiApplication();
#elif CRU_PLATFORM_OSX
  return new cru::platform::gui::osx::OsxUiApplication();
#else
  return nullptr;
#endif
}

CRU_PLATFORM_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::platform::graphics::win::direct::DirectGraphicsFactory();
#elif CRU_PLATFORM_OSX
  return new cru::platform::graphics::osx::quartz::QuartzGraphicsFactory();
#else
  return nullptr;
#endif
}

}  // namespace cru::platform::bootstrap
