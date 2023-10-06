#pragma once
#include "Base.h"

#include "cru/common/Base.h"
#include "cru/common/String.h"

namespace cru::platform {
struct CRU_PLATFORM_API IPlatformResource : virtual Interface {
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(IPlatformResource)

  virtual String GetPlatformId() const = 0;

  virtual String GetDebugString() { return String(); }
};
}  // namespace cru::platform
