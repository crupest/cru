#pragma once
#include "Base.h"

#include "cru/base/Base.h"
#include "cru/base/String.h"

namespace cru::platform {
struct CRU_PLATFORM_API IPlatformResource : virtual Interface {
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(IPlatformResource)

  virtual String GetPlatformId() const = 0;

  virtual String GetDebugString() { return String(); }

  virtual std::string GetPlatformIdUtf8() const;
};
}  // namespace cru::platform
