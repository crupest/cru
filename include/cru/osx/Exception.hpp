#pragma once
#include "cru/platform/Exception.hpp"

namespace cru::platform::osx {
class OsxException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::osx
