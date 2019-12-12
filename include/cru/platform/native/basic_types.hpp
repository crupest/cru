#pragma once
#include "cru/common/pre_config.hpp"

namespace cru::platform::native {
struct Dpi {
  float x;
  float y;
};

enum MouseButton : unsigned { Left = 0b1, Right = 0b10, Middle = 0b100 };
}  // namespace cru::platform::native
