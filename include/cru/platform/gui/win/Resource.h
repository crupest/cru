#pragma once
#include "Base.h"

#include <cru/platform/Base.h>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinNativeResource : public Object,
                                          public virtual IPlatformResource {
 public:
  static std::string kPlatformId;

 protected:
  WinNativeResource() = default;

 public:
  CRU_DELETE_COPY(WinNativeResource)
  CRU_DELETE_MOVE(WinNativeResource)

  ~WinNativeResource() override = default;

 public:
  std::string GetPlatformId() const final { return kPlatformId; }
};
}  // namespace cru::platform::gui::win
