#pragma once
#include "Base.h"
#include "cru/base/Base.h"
#include "cru/base/Exception.h"

#include <optional>
#include <string_view>

namespace cru::platform {
// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class CRU_PLATFORM_API PlatformNotMatchException : public PlatformException {
 public:
  PlatformNotMatchException(
      std::string resource_platform, std::string target_platform,
      std::optional<std::string_view> additional_message = std::nullopt);

  PlatformNotMatchException(
      StringView resource_platform, StringView target_platform,
      std::optional<StringView> additional_message = std::nullopt);

  ~PlatformNotMatchException() override;

  [[deprecated("Use GetResourcePlatformUtf8 instead.")]]
  String GetResourcePlatform() const;
  [[deprecated("Use GetTargetPlatform instead.")]]
  String GetTargetPlatform() const;

  std::string GetResourcePlatformUtf8() const { return resource_platform_; }
  std::string GetTargetPlatformUtf8() const { return target_platform_; }

 private:
  std::string resource_platform_;
  std::string target_platform_;
};

// This exception is thrown when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to
// this exception.
class CRU_PLATFORM_API ReuseException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_DESTRUCTOR(ReuseException)
};

class CRU_PLATFORM_API PlatformUnsupportedException : public PlatformException {
 public:
  PlatformUnsupportedException(
      std::string platform, std::string operation,
      std::optional<std::string_view> additional_message);

  PlatformUnsupportedException(StringView platform, StringView operation,
                               std::optional<StringView> additional_message);

  ~PlatformUnsupportedException() override;

  [[deprecated("Use GetPlatformUtf8 instead.")]]
  String GetPlatform() const;
  [[deprecated("Use GetOperationUtf8 instead.")]]
  String GetOperation() const;

  std::string GetPlatformUtf8() const { return platform_; }
  std::string GetOperationUtf8() const { return operation_; }

 private:
  std::string platform_;
  std::string operation_;
};
}  // namespace cru::platform
