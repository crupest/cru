#pragma once
#include "cru/common/base.hpp"
#include "cru/platform/graph/base.hpp"
#include "cru/platform/native/base.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace cru::ui {
//-------------------- region: import --------------------
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

//-------------------- region: forward declaration --------------------
class Window;
class Control;
class ClickDetector;

//-------------------- region: basic types --------------------
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

struct CornerRadius {
  constexpr CornerRadius()
      : left_top(), right_top(), left_bottom(), right_bottom() {}
  constexpr CornerRadius(const Point& value)
      : left_top(value),
        right_top(value),
        left_bottom(value),
        right_bottom(value) {}
  constexpr CornerRadius(Point left_top, Point right_top, Point left_bottom,
                         Point right_bottom)
      : left_top(left_top),
        right_top(right_top),
        left_bottom(left_bottom),
        right_bottom(right_bottom) {}

  Point left_top;
  Point right_top;
  Point left_bottom;
  Point right_bottom;
};

inline bool operator==(const CornerRadius& left, const CornerRadius& right) {
  return left.left_top == right.left_top &&
         left.left_bottom == right.left_bottom &&
         left.right_top == right.right_top &&
         left.right_bottom == right.right_bottom;
}

inline bool operator!=(const CornerRadius& left, const CornerRadius& right) {
  return !(left == right);
}

class CanvasPaintEventArgs {
 public:
  CanvasPaintEventArgs(platform::graph::IPainter* painter,
                       const Rect& paint_rect)
      : painter_(painter), paint_rect_(paint_rect) {}
  CRU_DEFAULT_COPY(CanvasPaintEventArgs)
  CRU_DEFAULT_MOVE(CanvasPaintEventArgs)
  ~CanvasPaintEventArgs() = default;

  platform::graph::IPainter* GetPainter() const { return painter_; }
  Rect GetPaintRect() const { return paint_rect_; }

 private:
  platform::graph::IPainter* painter_;
  Rect paint_rect_;
};

enum class FlexDirection {
  Horizontal,
  HorizontalReverse,
  Vertical,
  VertivalReverse
};

using FlexMainAlignment = Alignment;
using FlexCrossAlignment = Alignment;

struct FlexChildLayoutData {
  // nullopt stands for looking at my content
  std::optional<float> flex_basis = std::nullopt;
  float flex_grow = 0;
  float flex_shrink = 0;
  // nullopt stands for looking at parent's setting
  std::optional<FlexCrossAlignment> cross_alignment = std::nullopt;
};

struct StackChildLayoutData {
  Alignment horizontal = Alignment::Start;
  Alignment vertical = Alignment::Start;
};
}  // namespace cru::ui
