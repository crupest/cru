#pragma once
#include "cru/platform/native/Base.hpp"

// The dpi awareness needs to be implemented in the future. Currently we use 96
// as default.

namespace cru::platform::native::win {
inline platform::native::Dpi GetDpi() {
  return platform::native::Dpi{96.0f, 96.0f};
}
}  // namespace cru::platform::native::win
