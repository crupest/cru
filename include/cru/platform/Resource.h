#pragma once
#include "Base.h"

#include <cru/base/Base.h>

#include <string>

namespace cru::platform {
struct CRU_PLATFORM_API IPlatformResource : virtual Interface {
  virtual std::string GetPlatformId() const = 0;
  virtual std::string GetDebugString();
};
}  // namespace cru::platform
