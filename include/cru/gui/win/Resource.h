#pragma once
#include "Base.h"

#include "cru/Resource.h"

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinNativeResource : public Object,
                                          public virtual IPlatformResource {
 public:
  static String kPlatformId;

 protected:
  WinNativeResource() = default;

 public:

  ~WinNativeResource() override = default;

 public:
  String GetPlatformId() const final { return kPlatformId; }
};
}  // namespace cru::platform::gui::win
