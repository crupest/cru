#pragma once
#include "cru/common/Base.hpp"

#include <stdexcept>

namespace cru::platform {
class PlatformException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;  // inherit constructors
};

// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class UnsupportPlatformException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;  // inherit constructors
};

// This exception is thrown when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to
// this exception.
class ReuseException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;  // inherit constructors
};
}  // namespace cru::platform
