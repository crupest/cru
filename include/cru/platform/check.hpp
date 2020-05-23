#pragma once
#include "exception.hpp"
#include "resource.hpp"

#include <fmt/format.h>
#include <memory>
#include <type_traits>

namespace cru::platform {
template <typename TTarget>
TTarget* CheckPlatform(INativeResource* resource,
                       const std::string_view& target_platform) {
  Expects(resource);
  const auto result = dynamic_cast<TTarget*>(resource);
  if (result == nullptr) {
    throw UnsupportPlatformException(fmt::format(
        "Try to convert resource to target platform failed. Platform id of "
        "resource to convert: {} . Target platform id: {} .",
        resource->GetPlatformId(), target_platform));
  }
  return result;
}

template <typename TTarget, typename TSource>
std::shared_ptr<TTarget> CheckPlatform(
    const std::shared_ptr<TSource>& resource,
    const std::string_view& target_platform) {
  static_assert(std::is_base_of_v<INativeResource, TSource>,
                "TSource must be a subclass of INativeResource.");
  Expects(resource);
  const auto result = std::dynamic_pointer_cast<TTarget>(resource);
  if (result == nullptr) {
    throw UnsupportPlatformException(fmt::format(
        "Try to convert resource to target platform failed. Platform id of "
        "resource to convert: {} . Target platform id: {} .",
        resource->GetPlatformId(), target_platform));
  }
  return result;
}
}  // namespace cru::platform
