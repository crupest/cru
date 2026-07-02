#pragma once
#include "cru/platform/graphics/Factory.h"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_GRAPHICS_BOOTSTRAP_EXPORT_API
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API
#endif

namespace cru::platform::bootstrap {
/**
 * @brief Get the list of available graphics platforms, in order of preference.
 */
CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API
std::vector<std::string> GetAvailableGraphicsPlatforms();

using GraphicsFactoryCreator =
    std::function<cru::platform::graphics::IGraphicsFactory*()>;

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API
std::optional<GraphicsFactoryCreator> GetGraphicsFactoryCreator(
    std::string_view platform_name);

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory(const std::vector<std::string>& platforms);

CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory();
}  // namespace cru::platform::bootstrap
