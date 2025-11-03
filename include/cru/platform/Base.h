#pragma once

#include <cru/base/Base.h>

#include <string>

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_EXPORT_API
#define CRU_PLATFORM_API __declspec(dllexport)
#else
#define CRU_PLATFORM_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_API
#endif


namespace cru::platform {
struct CRU_PLATFORM_API IPlatformResource : virtual Interface {
  virtual std::string GetPlatformId() const = 0;
  virtual std::string GetDebugString();
};
}  // namespace cru::platform
