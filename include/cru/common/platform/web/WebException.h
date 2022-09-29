#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_EMSCRIPTEN

#include "../Exception.h"

namespace cru::platform::web {
class WebException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::web

#endif
