#include "cru/platform/Exception.h"
#include "cru/base/Format.h"

#include <optional>

namespace cru::platform {
PlatformNotMatchException::PlatformNotMatchException(
    String resource_platform, String target_platform,
    std::optional<StringView> additional_message)
    : resource_platform_(std::move(resource_platform)),
      target_platform_(std::move(target_platform)) {
  SetMessage(
      Format(u"Resource platform '{}' does not match target platform '{}'.",
             resource_platform_, target_platform_));

  AppendMessage(additional_message);
}

PlatformNotMatchException::~PlatformNotMatchException() {}

String PlatformNotMatchException::GetResourcePlatform() const {
  return resource_platform_;
}

String PlatformNotMatchException::GetTargetPlatform() const {
  return target_platform_;
}

PlatformUnsupportedException::PlatformUnsupportedException(
    String platform, String operation,
    std::optional<StringView> additional_message)
    : platform_(std::move(platform)), operation_(std::move(operation)) {
  SetMessage(Format(u"Operation '{}' is not supported on platform '{}'.",
                    operation_, platform_));
  AppendMessage(additional_message);
}

PlatformUnsupportedException::~PlatformUnsupportedException() {}

String PlatformUnsupportedException::GetPlatform() const { return platform_; }

String PlatformUnsupportedException::GetOperation() const { return operation_; }

}  // namespace cru::platform
