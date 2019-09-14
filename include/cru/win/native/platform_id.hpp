#pragma once
#include <cru/platform/native_resource.hpp>

#include <stdexcept>
#include <string_view>

namespace cru::platform::native::win {
constexpr std::wstring_view win_platform_id = L"Windows";

inline bool IsWindowsResource(NativeResource* resource) {
  return resource->GetPlatformId() == win_platform_id;
}

}  // namespace cru::platform::native::win

#define CRU_PLATFORMID_IMPLEMENT_WIN                      \
  std::wstring_view GetPlatformId() const override {      \
    return ::cru::platform::native::win::win_platform_id; \
  }
