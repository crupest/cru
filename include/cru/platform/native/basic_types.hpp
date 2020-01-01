#pragma once
#include "cru/common/base.hpp"
#include "cru/common/bitmask.hpp"

namespace cru::platform::native {
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

}  // namespace cru::platform::native
