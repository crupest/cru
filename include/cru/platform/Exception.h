#pragma once
#include "Base.h"
#include "cru/common/Base.h"
#include "cru/common/Exception.h"
#include "cru/common/platform/Exception.h"

#include <optional>

namespace cru::platform {
// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class CRU_PLATFORM_API PlatformNotMatchException : public PlatformException {
 public:
  PlatformNotMatchException(
      String resource_platform, String target_platform,
      std::optional<StringView> additional_message = std::nullopt);

  ~PlatformNotMatchException() override;

  String GetResourcePlatform() const;
  String GetTargetPlatform() const;

 private:
  String resource_platform_;
  String target_platform_;
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
  PlatformUnsupportedException(String platform, String operation,
                               std::optional<StringView> additional_message);

  ~PlatformUnsupportedException() override;

  String GetPlatform() const;
  String GetOperation() const;

 private:
  String platform_;
  String operation_;
};
}  // namespace cru::platform
