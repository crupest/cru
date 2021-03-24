#pragma once
#include "Base.hpp"

#include "cru/platform/Resource.hpp"

namespace cru::platform::gui::win {
class WinNativeResource : public Object, public virtual INativeResource {
 public:
  static constexpr std::u16string_view k_platform_id = u"Windows";

 protected:
  WinNativeResource() = default;

 public:
  CRU_DELETE_COPY(WinNativeResource)
  CRU_DELETE_MOVE(WinNativeResource)

  ~WinNativeResource() override = default;

 public:
  std::u16string_view GetPlatformId() const final { return k_platform_id; }
};
}  // namespace cru::platform::gui::win
