#pragma once

#include "../graphic_base.hpp"
#include "basic_types.hpp"

namespace cru::platform::native {
struct NativeMouseButtonEventArgs {
  MouseButton button;
  Point point;
};

enum class FocusChangeType { Gain, Lost };

enum class MouseEnterLeaveType { Enter, Leave };

}  // namespace cru::platform::native
