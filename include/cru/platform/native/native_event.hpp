#pragma once
#include "cru/common/base.hpp"

#include "basic_types.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform::native {
struct NativeMouseButtonEventArgs {
  MouseButton button;
  ui::Point point;
};
}  // namespace cru::platform::native
