#pragma once
#include "cru/common/pre_config.hpp"

namespace cru::platform::native {
struct Dpi {
  float x;
  float y;
};

enum MouseButton : unsigned { Left = 1, Right = 2, Middle = 4 };
}  // namespace cru::platform::native
