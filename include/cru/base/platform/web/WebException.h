#pragma once

#ifdef CRU_PLATFORM_EMSCRIPTEN

#include "../../Base.h"

namespace cru::platform::web {
class WebException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::web

#endif
