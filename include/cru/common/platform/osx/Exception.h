#pragma once
#include "../../PreConfig.h"

#ifndef CRU_PLATFORM_OSX
#error "This file can only be included on osx."
#endif

#include "../Exception.h"

namespace cru::platform::osx {
class OsxException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::osx
