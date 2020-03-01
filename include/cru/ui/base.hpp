#pragma once
#include "cru/common/base.hpp"
#include "cru/platform/graph_base.hpp"
#include "cru/platform/matrix.hpp"
#include "cru/platform/native/basic_types.hpp"

namespace cru::ui {
using cru::platform::Color;
using cru::platform::Ellipse;
using cru::platform::Matrix;
using cru::platform::Point;
using cru::platform::Rect;
using cru::platform::RoundedRect;
using cru::platform::Size;
using cru::platform::TextRange;
using cru::platform::Thickness;
using cru::platform::native::MouseButton;

namespace mouse_buttons {
using cru::platform::native::mouse_buttons::left;
using cru::platform::native::mouse_buttons::middle;
using cru::platform::native::mouse_buttons::right;
}  // namespace mouse_buttons

namespace colors {
using cru::platform::colors::black;
using cru::platform::colors::skyblue;
using cru::platform::colors::white;
}  // namespace colors

namespace internal {
constexpr int align_start = 0;
constexpr int align_end = align_start + 1;
constexpr int align_center = align_end + 1;
constexpr int align_stretch = align_center + 1;
}  // namespace internal

enum class Alignment {
  Start = internal::align_start,
  End = internal::align_end,
  Center = internal::align_center,
  Stretch = internal::align_stretch
};
}  // namespace cru::ui
