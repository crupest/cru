#pragma once
#include <cru/base/Base.h>
#include <cru/platform/Color.h>
#include <cru/platform/GraphicsBase.h>
#include <cru/platform/Matrix.h>
#include <cru/platform/gui/Input.h>

#ifdef CRU_IS_DLL
#ifdef CRU_UI_EXPORT_API
#define CRU_UI_API __declspec(dllexport)
#else
#define CRU_UI_API __declspec(dllimport)
#endif
#else
#define CRU_UI_API
#endif

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
using cru::platform::gui::MouseButtons;

namespace colors = cru::platform::colors;

//-------------------- region: forward declaration --------------------

namespace controls {
class Control;
class ControlHost;
}  // namespace controls

//-------------------- region: basic types --------------------
enum class Direction { Horizontal, Vertical };
enum class Alignment { Start, End, Center, Stretch };

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

  CornerRadius& SetAll(const Point& point) {
    left_top = right_top = left_bottom = right_bottom = point;
    return *this;
  }

  bool operator==(const CornerRadius& other) const = default;

  Point left_top;
  Point right_top;
  Point left_bottom;
  Point right_bottom;
};

}  // namespace cru::ui
