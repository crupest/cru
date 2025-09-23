#pragma once

#include <cru/base/Exception.h>

#include "../../Resource.h"

namespace cru::platform::gui::xcb {
class XcbResource : public Object, public virtual IPlatformResource {
 public:
  static constexpr const char16_t* kPlatformId = u"XCB";

 protected:
  XcbResource() = default;

 public:
  String GetPlatformId() const final { return String(kPlatformId); }
};

class XcbException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::gui::xcb
