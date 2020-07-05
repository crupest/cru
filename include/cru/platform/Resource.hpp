#pragma once
#include "cru/common/Base.hpp"

#include <string_view>

namespace cru::platform {
struct INativeResource : virtual Interface {
  virtual std::u16string_view GetPlatformId() const = 0;
};
}  // namespace cru::platform
