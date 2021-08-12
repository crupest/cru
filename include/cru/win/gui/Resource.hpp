#pragma once
#include "Base.hpp"

#include "cru/platform/Resource.hpp"

namespace cru::platform::gui::win {
class WinNativeResource : public Object, public virtual IPlatformResource {
 public:
  static String kPlatformId;

 protected:
  WinNativeResource() = default;

 public:
  CRU_DELETE_COPY(WinNativeResource)
  CRU_DELETE_MOVE(WinNativeResource)

  ~WinNativeResource() override = default;

 public:
  String GetPlatformId() const final { return kPlatformId; }
};
}  // namespace cru::platform::gui::win
