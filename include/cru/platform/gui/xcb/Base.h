#pragma once

#include <cru/base/Exception.h>
#include <cru/platform/Resource.h>
#include <cru/platform/gui/Base.h>

namespace cru::platform::gui::xcb {
class XcbResource : public Object, public virtual IPlatformResource {
 public:
  static constexpr const char* kPlatformId = "XCB";

 protected:
  XcbResource() = default;

 public:
  std::string GetPlatformId() const final { return std::string(kPlatformId); }
};

class XcbException : public PlatformException {
 public:
  using PlatformException::PlatformException;
};
}  // namespace cru::platform::gui::xcb
