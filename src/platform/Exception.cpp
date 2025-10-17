#include "cru/platform/Exception.h"
#include "cru/base/Exception.h"

#include <format>
#include <optional>
#include <string_view>

namespace cru::platform {
PlatformNotMatchException::PlatformNotMatchException(
    std::string resource_platform, std::string target_platform,
    std::optional<std::string_view> additional_message)
    : PlatformException(std::format(
          "Resource platform '{}' does not match target platform '{}'.",
          resource_platform_, target_platform_)),
      resource_platform_(std::move(resource_platform)),
      target_platform_(std::move(target_platform)) {
  AppendMessage(additional_message);
}

PlatformNotMatchException::~PlatformNotMatchException() {}

PlatformUnsupportedException::PlatformUnsupportedException(
    std::string platform, std::string operation,
    std::optional<std::string_view> additional_message)
    : PlatformException(
          std::format("Operation '{}' is not supported on platform '{}'.",
                      operation, platform)),
      platform_(std::move(platform)),
      operation_(std::move(operation)) {
  AppendMessage(additional_message);
}

PlatformUnsupportedException::~PlatformUnsupportedException() {}
}  // namespace cru::platform
