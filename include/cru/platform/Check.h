#pragma once
#include "Exception.h"
#include "Resource.h"

#include "cru/base/String.h"

#include <memory>
#include <type_traits>

namespace cru::platform {
template <typename TTarget>
TTarget* CheckPlatform(IPlatformResource* resource,
                       const String& target_platform) {
  if (resource == nullptr) return nullptr;
  const auto result = dynamic_cast<TTarget*>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        u"Try to convert resource to target platform failed.");
  }
  return result;
}

template <typename TTarget, typename TSource>
std::shared_ptr<TTarget> CheckPlatform(const std::shared_ptr<TSource>& resource,
                                       const String& target_platform) {
  if (resource == nullptr) return nullptr;
  static_assert(std::is_base_of_v<IPlatformResource, TSource>,
                "TSource must be a subclass of INativeResource.");
  const auto result = std::dynamic_pointer_cast<TTarget>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        u"Try to convert resource to target platform failed.");
  }
  return result;
}

template <typename TTarget>
TTarget* CheckPlatform(IPlatformResource* resource,
                       std::string target_platform) {
  if (resource == nullptr) return nullptr;
  const auto result = dynamic_cast<TTarget*>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformIdUtf8(), target_platform,
        "Try to convert resource to target platform failed.");
  }
  return result;
}

template <typename TTarget, typename TSource>
std::shared_ptr<TTarget> CheckPlatform(const std::shared_ptr<TSource>& resource,
                                       std::string target_platform) {
  if (resource == nullptr) return nullptr;
  static_assert(std::is_base_of_v<IPlatformResource, TSource>,
                "TSource must be a subclass of IPlatformResource.");
  const auto result = std::dynamic_pointer_cast<TTarget>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformIdUtf8(), target_platform,
        "Try to convert resource to target platform failed.");
  }
  return result;
}
}  // namespace cru::platform
