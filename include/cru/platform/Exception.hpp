#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Exception.hpp"

namespace cru::platform {
class CRU_PLATFORM_API PlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_COPY(PlatformException)
  CRU_DEFAULT_MOVE(PlatformException)

  CRU_DEFAULT_DESTRUCTOR(PlatformException)
};

// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class CRU_PLATFORM_API UnsupportPlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_COPY(UnsupportPlatformException)
  CRU_DEFAULT_MOVE(UnsupportPlatformException)

  CRU_DEFAULT_DESTRUCTOR(UnsupportPlatformException)
};

// This exception is thrown when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to
// this exception.
class CRU_PLATFORM_API ReuseException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_COPY(ReuseException)
  CRU_DEFAULT_MOVE(ReuseException)

  CRU_DEFAULT_DESTRUCTOR(ReuseException)
};
}  // namespace cru::platform
