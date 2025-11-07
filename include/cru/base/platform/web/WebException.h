#pragma once

#ifdef __EMSCRIPTEN__

#include "../../Base.h"

namespace cru::platform::web {
class WebException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::web

#endif
