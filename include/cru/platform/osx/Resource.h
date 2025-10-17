#pragma once
#include "cru/platform/Resource.h"

namespace cru::platform::osx {
class OsxResource : public Object, public virtual IPlatformResource {
 public:
  std::string GetPlatformId() const override { return "OSX"; }
};
}  // namespace cru::platform::osx
