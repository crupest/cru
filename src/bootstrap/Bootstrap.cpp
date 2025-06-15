#include "cru/bootstrap/Bootstrap.h"

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/graphics/direct2d/Factory.h"
#include "cru/gui/win/UiApplication.h"
#elif defined(CRU_PLATFORM_OSX)
#include "cru/graphics/quartz/Factory.h"
#include "cru/gui/osx/UiApplication.h"
#elif defined(CRU_PLATFORM_EMSCRIPTEN)
// TODO: Go fill this!
#else
#include "cru/graphics/cairo/CairoGraphicsFactory.h"
#endif

namespace cru::platform::bootstrap {
cru::platform::gui::IUiApplication* CreateUiApplication() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::platform::gui::win::WinUiApplication();
#elif defined(CRU_PLATFORM_OSX)
  return new cru::platform::gui::osx::OsxUiApplication();
#else
  return nullptr;
#endif
}

CRU_PLATFORM_BOOTSTRAP_API cru::graphics::IGraphicsFactory*
CreateGraphicsFactory() {
#ifdef CRU_PLATFORM_WINDOWS
  return new cru::graphics::direct2d::DirectGraphicsFactory();
#elif defined(CRU_PLATFORM_OSX)
  return new cru::graphics::quartz::QuartzGraphicsFactory();
#elif defined(CRU_PLATFORM_EMSCRIPTEN)
  return nullptr; // TODO: Change this.
#else
  return new cru::graphics::cairo::CairoGraphicsFactory();
#endif
}

}  // namespace cru::platform::bootstrap
