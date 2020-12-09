#pragma once
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/gui/Base.hpp"

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
using cru::platform::gui::MouseButton;

namespace mouse_buttons = cru::platform::gui::mouse_buttons;

namespace colors = cru::platform::colors;

//-------------------- region: forward declaration --------------------

namespace controls {
class Window;
class Control;
}  // namespace controls

namespace host {
class WindowHost;
}

namespace render {
class RenderObject;
}

namespace style {
class StyleRuleSet;
class StyleRuleSetBind;
}

//-------------------- region: basic types --------------------
namespace internal {
constexpr int align_start = 0;
constexpr int align_end = align_start + 1;
constexpr int align_center = align_end + 1;
}  // namespace internal

enum class Alignment {
  Start = internal::align_start,
  End = internal::align_end,
  Center = internal::align_center,
};

struct CornerRadius {
  constexpr CornerRadius()
      : left_top(), right_top(), left_bottom(), right_bottom() {}
  constexpr CornerRadius(const float& value)
      : CornerRadius(Point{value, value}) {}
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
  CanvasPaintEventArgs(platform::graphics::IPainter* painter,
                       const Size& paint_size)
      : painter_(painter), paint_size_(paint_size) {}
  CRU_DEFAULT_COPY(CanvasPaintEventArgs)
  CRU_DEFAULT_MOVE(CanvasPaintEventArgs)
  ~CanvasPaintEventArgs() = default;

  platform::graphics::IPainter* GetPainter() const { return painter_; }
  Size GetPaintSize() const { return paint_size_; }

 private:
  platform::graphics::IPainter* painter_;
  Size paint_size_;
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
  float expand_factor = 0;
  float shrink_factor = 1;
  // nullopt stands for looking at parent's setting
  std::optional<FlexCrossAlignment> cross_alignment = std::nullopt;
};

struct StackChildLayoutData {
  Alignment horizontal = Alignment::Start;
  Alignment vertical = Alignment::Start;
};
}  // namespace cru::ui
