#pragma once
#include "cru/common/pre_config.hpp"

#include <stdexcept>

namespace cru::platform {
class PlatformException : public std::runtime_error {
 public:
  using runtime_error::runtime_error; // inherent constructors
};
}  // namespace cru::platform
