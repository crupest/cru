
#pragma once

#include <cru/base/Exception.h>

#include "../../Resource.h"

namespace cru::platform::gui::sdl {
class SdlResource : public Object, public virtual IPlatformResource {
 public:
  static constexpr const char16_t* kPlatformId = u"SDL";

 protected:
  SdlResource() = default;

 public:
  String GetPlatformId() const final { return String(kPlatformId); }
};

class SdlException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::gui::xcb
