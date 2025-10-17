
#pragma once

#include <cru/base/Exception.h>

#include "../../Resource.h"

namespace cru::platform::gui::sdl {
class SdlResource : public Object, public virtual IPlatformResource {
 public:
  static constexpr const char* kPlatformId = "SDL";

 protected:
  SdlResource() = default;

 public:
  std::string GetPlatformId() const final { return kPlatformId; }
};

class SdlException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::gui::xcb
