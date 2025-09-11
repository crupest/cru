#pragma once
#include "cru/platform/graphics/Factory.h"

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
CRU_PLATFORM_GRAPHICS_BOOTSTRAP_API cru::platform::graphics::IGraphicsFactory*
CreateGraphicsFactory();
}  // namespace cru::platform::bootstrap
