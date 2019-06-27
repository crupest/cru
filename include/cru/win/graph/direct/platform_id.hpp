#pragma once
#include <cru/platform/native_resource.hpp>

#include <string_view>

namespace cru::platform::graph::win::direct {
constexpr std::wstring_view win_direct_platform_id = L"Windows Direct";

inline bool IsDirectResource(NativeResource* resource) {
  return resource->GetPlatformId() == win_direct_platform_id;
}

}  // namespace cru::platform::graph::win::direct

#define CRU_PLATFORMID_IMPLEMENT_DIRECT                                 \
  std::wstring_view GetPlatformId() const override {                    \
    return ::cru::platform::graph::win::direct::win_direct_platform_id; \
  }
