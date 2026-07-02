#include "cru/platform/bootstrap/GraphicsBootstrap.h"

#include <string>
#include <unordered_map>
#include "cru/base/StringUtil.h"

#if defined(_WIN32)
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_DIRECT2D 1
#endif

#if defined(__APPLE__)
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_QUARTZ 1
#endif

#if defined(__unix) || (defined(_WIN32) && CRU_BUILD_CAIRO_ON_WINDOWS)
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_CAIRO 1
#endif

#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_DIRECT2D
#include "cru/platform/graphics/direct2d/Factory.h"
#endif

#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_QUARTZ
#include "cru/platform/graphics/quartz/Factory.h"
#endif

#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_CAIRO
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#endif

namespace cru::platform::bootstrap {
CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API
std::vector<std::string> GetAvailableGraphicsPlatforms() {
#if defined(_WIN32)
  return {"Direct2D", "Cairo"};
#elif defined(__APPLE__)
  // TODO: Add Cairo.
  return {"Quartz"};
#elif defined(__unix)
  return {"Cairo"};
#else
  NotImplemented();
#endif
}

static std::unordered_map<std::string, GraphicsFactoryCreator>
    kGraphicsFactoryCreators = {
#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_DIRECT2D
        {"Direct2D",
         []() {
           return new cru::platform::graphics::direct2d::
               DirectGraphicsFactory();
         }},
#endif
#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_QUARTZ
        {"Quartz",
         []() {
           return new cru::platform::graphics::quartz::QuartzGraphicsFactory();
         }},
#endif
#if CRU_PLATFORM_GRAPHICS_BOOTSTRAP_USE_CAIRO
        {"Cairo",
         []() {
           return new cru::platform::graphics::cairo::CairoGraphicsFactory();
         }},
#endif
};

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API
std::optional<std::function<cru::platform::graphics::IGraphicsFactory*()>>
GetGraphicsFactoryCreator(std::string_view platform_name) {
  auto it = kGraphicsFactoryCreators.find(std::string(platform_name));
  if (it != kGraphicsFactoryCreators.end()) {
    return it->second;
  }
  return std::nullopt;
}

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory(const std::vector<std::string>& platforms) {
  for (auto& platform : platforms) {
    auto creator = GetGraphicsFactoryCreator(platform);
    if (creator) {
      return (*creator)();
    }
  }
  throw NotImplementedException("No available graphics platform.");
}

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory() {
  auto env_platform = std::getenv("CRU_GRAPHICS_PLATFORM");
  std::vector<std::string> platforms;
  if (env_platform) {
    platforms =
        cru::string::Split(env_platform, ";",
                           cru::string::SplitOptions::RemoveEmptyAndSpace |
                               cru::string::SplitOptions::Trim);
  } else {
    platforms = GetAvailableGraphicsPlatforms();
  }

  return CreateGraphicsFactory(platforms);
}
}  // namespace cru::platform::bootstrap
