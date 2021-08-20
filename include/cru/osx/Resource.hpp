#pragma once
#include "cru/platform/Resource.hpp"

namespace cru::platform::osx {
class OsxResource : public Object, public virtual IPlatformResource {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(OsxResource)
  CRU_DELETE_COPY(OsxResource)
  CRU_DELETE_MOVE(OsxResource)

  String GetPlatformId() const override { return u"OSX"; }
};
}  // namespace cru::platform::osx
