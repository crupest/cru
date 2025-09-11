#include "cru/platform/bootstrap/GraphicsBootstrap.h"

#if defined(_WIN32)
#include "cru/platform/graphics/direct2d/Factory.h"
#elif defined(__APPLE__)
#include "cru/platform/graphics/quartz/Factory.h"
#elif defined(__unix)
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#else
#endif

namespace cru::platform::bootstrap {
CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory() {
#if defined(_WIN32)
  return new cru::platform::graphics::direct2d::DirectGraphicsFactory();
#elif defined(__APPLE__)
  return new cru::platform::graphics::quartz::QuartzGraphicsFactory();
#elif defined(__unix)
  return new cru::platform::graphics::cairo::CairoGraphicsFactory();
#else
  NotImplemented();
#endif
}
}  // namespace cru::platform::bootstrap
