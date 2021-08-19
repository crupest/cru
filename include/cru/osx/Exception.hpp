#pragma once
#include "cru/platform/Exception.hpp"

namespace cru::platform::osx {
  class OsxException : PlatformException {
    public:
    using PlatformException::PlatformException;
  };
}
