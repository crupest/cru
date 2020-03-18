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
using cru::platform::colors::silver;
using cru::platform::colors::gray;
using cru::platform::colors::white;
using cru::platform::colors::maroon;
using cru::platform::colors::red;
using cru::platform::colors::purple;
using cru::platform::colors::fuchsia;
using cru::platform::colors::green;
using cru::platform::colors::lime;
using cru::platform::colors::olive;
using cru::platform::colors::yellow;
using cru::platform::colors::navy;
using cru::platform::colors::blue;
using cru::platform::colors::teal;
using cru::platform::colors::aqua;
using cru::platform::colors::orange;
using cru::platform::colors::aliceblue;
using cru::platform::colors::antiquewhite;
using cru::platform::colors::aquamarine;
using cru::platform::colors::azure;
using cru::platform::colors::beige;
using cru::platform::colors::bisque;
using cru::platform::colors::blanchedalmond;
using cru::platform::colors::blueviolet;
using cru::platform::colors::brown;
using cru::platform::colors::burlywood;
using cru::platform::colors::cadetblue;
using cru::platform::colors::chartreuse;
using cru::platform::colors::chocolate;
using cru::platform::colors::coral;
using cru::platform::colors::cornflowerblue;
using cru::platform::colors::cornsilk;
using cru::platform::colors::crimson;
using cru::platform::colors::cyan;
using cru::platform::colors::darkblue;
using cru::platform::colors::darkcyan;
using cru::platform::colors::darkgoldenrod;
using cru::platform::colors::darkgray;
using cru::platform::colors::darkgreen;
using cru::platform::colors::darkgrey;
using cru::platform::colors::darkkhaki;
using cru::platform::colors::darkmagenta;
using cru::platform::colors::darkolivegreen;
using cru::platform::colors::darkorange;
using cru::platform::colors::darkorchid;
using cru::platform::colors::darkred;
using cru::platform::colors::darksalmon;
using cru::platform::colors::darkseagreen;
using cru::platform::colors::darkslateblue;
using cru::platform::colors::darkslategray;
using cru::platform::colors::darkslategrey;
using cru::platform::colors::darkturquoise;
using cru::platform::colors::darkviolet;
using cru::platform::colors::deeppink;
using cru::platform::colors::deepskyblue;
using cru::platform::colors::dimgray;
using cru::platform::colors::dimgrey;
using cru::platform::colors::dodgerblue;
using cru::platform::colors::firebrick;
using cru::platform::colors::floralwhite;
using cru::platform::colors::forestgreen;
using cru::platform::colors::gainsboro;
using cru::platform::colors::ghostwhite;
using cru::platform::colors::gold;
using cru::platform::colors::goldenrod;
using cru::platform::colors::greenyellow;
using cru::platform::colors::grey;
using cru::platform::colors::honeydew;
using cru::platform::colors::hotpink;
using cru::platform::colors::indianred;
using cru::platform::colors::indigo;
using cru::platform::colors::ivory;
using cru::platform::colors::khaki;
using cru::platform::colors::lavender;
using cru::platform::colors::lavenderblush;
using cru::platform::colors::lawngreen;
using cru::platform::colors::lemonchiffon;
using cru::platform::colors::lightblue;
using cru::platform::colors::lightcoral;
using cru::platform::colors::lightcyan;
using cru::platform::colors::lightgoldenrodyellow;
using cru::platform::colors::lightgray;
using cru::platform::colors::lightgreen;
using cru::platform::colors::lightgrey;
using cru::platform::colors::lightpink;
using cru::platform::colors::lightsalmon;
using cru::platform::colors::lightseagreen;
using cru::platform::colors::lightskyblue;
using cru::platform::colors::lightslategray;
using cru::platform::colors::lightslategrey;
using cru::platform::colors::lightsteelblue;
using cru::platform::colors::lightyellow;
using cru::platform::colors::limegreen;
using cru::platform::colors::linen;
using cru::platform::colors::magenta;
using cru::platform::colors::mediumaquamarine;
using cru::platform::colors::mediumblue;
using cru::platform::colors::mediumorchid;
using cru::platform::colors::mediumpurple;
using cru::platform::colors::mediumseagreen;
using cru::platform::colors::mediumslateblue;
using cru::platform::colors::mediumspringgreen;
using cru::platform::colors::mediumturquoise;
using cru::platform::colors::mediumvioletred;
using cru::platform::colors::midnightblue;
using cru::platform::colors::mintcream;
using cru::platform::colors::mistyrose;
using cru::platform::colors::moccasin;
using cru::platform::colors::navajowhite;
using cru::platform::colors::oldlace;
using cru::platform::colors::olivedrab;
using cru::platform::colors::orangered;
using cru::platform::colors::orchid;
using cru::platform::colors::palegoldenrod;
using cru::platform::colors::palegreen;
using cru::platform::colors::paleturquoise;
using cru::platform::colors::palevioletred;
using cru::platform::colors::papayawhip;
using cru::platform::colors::peachpuff;
using cru::platform::colors::peru;
using cru::platform::colors::pink;
using cru::platform::colors::plum;
using cru::platform::colors::powderblue;
using cru::platform::colors::rosybrown;
using cru::platform::colors::royalblue;
using cru::platform::colors::saddlebrown;
using cru::platform::colors::salmon;
using cru::platform::colors::sandybrown;
using cru::platform::colors::seagreen;
using cru::platform::colors::seashell;
using cru::platform::colors::sienna;
using cru::platform::colors::skyblue;
using cru::platform::colors::slateblue;
using cru::platform::colors::slategray;
using cru::platform::colors::slategrey;
using cru::platform::colors::snow;
using cru::platform::colors::springgreen;
using cru::platform::colors::steelblue;
using cru::platform::colors::tan;
using cru::platform::colors::thistle;
using cru::platform::colors::tomato;
using cru::platform::colors::turquoise;
using cru::platform::colors::violet;
using cru::platform::colors::wheat;
using cru::platform::colors::whitesmoke;
using cru::platform::colors::yellowgreen;
using cru::platform::colors::rebeccapurple;
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
