#pragma once
#include "../../PreConfig.h"
#ifdef CRU_PLATFORM_OSX

#include "../Exception.h"

namespace cru::platform::osx {
class OsxException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::osx

#endif
