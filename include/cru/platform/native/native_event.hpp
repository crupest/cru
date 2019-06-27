#pragma once

#include "../graphic_base.hpp"
#include "basic_types.hpp"

namespace cru::platform::native {
struct NativeMouseButtonEventArgs {
  MouseButton button;
  Point point;
};
}  // namespace cru::platform::native
