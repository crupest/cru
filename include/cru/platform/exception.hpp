#pragma once
#include "cru/common/pre_config.hpp"

#include <stdexcept>

namespace cru::platform {
class PlatformException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;  // inherit constructors
};

// This exception is throwed when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to this
// exception.
class ReuseException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;  // inherit constructors
};
}  // namespace cru::platform
