#pragma once
#include "cru/common/base.hpp"

#include <string_view>

namespace cru::platform {
struct INativeResource : virtual Interface {
  virtual std::string_view GetPlatformId() const = 0;
};
}  // namespace cru::platform
