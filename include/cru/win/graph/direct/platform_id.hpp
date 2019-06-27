#pragma once
#include <cru/platform/native_resource.hpp>

#include <stdexcept>
#include <string_view>

namespace cru::platform::graph::win::direct {
constexpr std::wstring_view platform_id = L"Windows Direct";

bool IsDirectResource(NativeResource* resource) {
  return resource->GetPlatformId() == platform_id;
}

}  // namespace cru::platform::graph::win::direct

#define CRU_PLATFORMID_IMPLEMENT_DIRECT                      \
  std::wstring_view GetPlatformId() const override {         \
    return ::cru::platform::graph::win::direct::platform_id; \
  }
