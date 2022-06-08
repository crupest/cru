#include "cru/platform/bootstrap/Bootstrap.h"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/gui/win/UiApplication.h"
#elifdef CRU_PLATFORM_OSX
#include "cru/platform/graphics/quartz/Factory.h"
#include "cru/platform/gui/osx/UiApplication.h"
#else
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
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
  return new cru::platform::graphics::direct2d::DirectGraphicsFactory();
#elif CRU_PLATFORM_OSX
  return new cru::platform::graphics::quartz::QuartzGraphicsFactory();
#else
  return new cru::platform::graphics::cairo::CairoGraphicsFactory();
#endif
}

}  // namespace cru::platform::bootstrap
