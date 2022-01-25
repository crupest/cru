#pragma once
#include "../../PreConfig.hpp"
#ifdef CRU_PLATFORM_OSX

#include "../Exception.hpp"

namespace cru::platform::osx {
class OsxException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::osx

#endif
