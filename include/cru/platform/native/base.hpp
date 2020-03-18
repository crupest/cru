#pragma once
#include "cru/common/base.hpp"
#include "cru/common/bitmask.hpp"
#include "cru/platform/graph/base.hpp"

namespace cru::platform::native {
struct ICursor;
struct ICursorManager;
struct IUiApplication;
struct INativeWindow;
struct INativeWindowResolver;

struct Dpi {
  float x;
  float y;
};

namespace details {
struct TagMouseButton {};
}  // namespace details

using MouseButton = Bitmask<details::TagMouseButton>;

namespace mouse_buttons {
constexpr MouseButton left{0b1};
constexpr MouseButton middle{0b10};
constexpr MouseButton right{0b100};
}  // namespace mouse_buttons

enum class SystemCursorType {
  Arrow,
  Hand,
};

struct NativeMouseButtonEventArgs {
  MouseButton button;
  Point point;
};

enum class FocusChangeType { Gain, Lost };

enum class MouseEnterLeaveType { Enter, Leave };
}  // namespace cru::platform::native
