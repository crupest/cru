#pragma once
#include "cru/osx/Resource.hpp"

namespace cru::platform::graphics::osx::quartz {
class OsxQuartzResource : public platform::osx::OsxResource {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(OsxQuartzResource)
  CRU_DELETE_COPY(OsxQuartzResource)
  CRU_DELETE_MOVE(OsxQuartzResource)

 public:
  String GetPlatformId() const override { return u"OSX Quartz"; }
};
}  // namespace cru::platform::graphics::osx::quartz
