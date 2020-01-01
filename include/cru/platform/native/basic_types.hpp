#pragma once
#include "cru/common/base.hpp"

namespace cru::platform::native {
struct Dpi {
  float x;
  float y;
};

enum class MouseButton : unsigned { Left = 0b1, Right = 0b10, Middle = 0b100 };
}  // namespace cru::platform::native

CRU_ENABLE_BITMASK_OPERATORS(::cru::platform::native::MouseButton)
