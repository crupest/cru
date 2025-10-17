#pragma once
#include "Base.h"

#include <cru/base/Range.h>
#include <cru/base/StringUtil.h>

#include <format>
#include <limits>

namespace cru::platform {
struct Size;

struct Point final {
  constexpr Point() = default;
  constexpr Point(const float x, const float y) : x(x), y(y) {}
  explicit constexpr Point(const Size& size);

  constexpr Point& operator+=(const Point& other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
  }

  constexpr Point Negate() const { return Point(-x, -y); }

  std::string ToString() const {
    return std::format("Point(x: {}, y: {})", x, y);
  }

  float x = 0;
  float y = 0;
};

constexpr Point operator+(const Point& left, const Point& right) {
  return Point(left.x + right.x, left.y + right.y);
}

constexpr Point operator-(const Point& left, const Point& right) {
  return Point(left.x - right.x, left.y - right.y);
}

constexpr bool operator==(const Point& left, const Point& right) {
  return left.x == right.x && left.y == right.y;
}

struct Size final {
  static CRU_PLATFORM_API const Size kMax;
  static CRU_PLATFORM_API const Size kZero;

  constexpr Size() = default;
  constexpr Size(const float width, const float height)
      : width(width), height(height) {}
  explicit constexpr Size(const Point& point)
      : width(point.x), height(point.y) {}

  constexpr static Size Infinite() {
    return Size{std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()};
  }

  std::string ToString() const {
    return std::format("Size(width: {}, height: {})", width, height);
  }

  float width = 0;
  float height = 0;
};

constexpr Point::Point(const Size& size) : x(size.width), y(size.height) {}

constexpr Size operator+(const Size& left, const Size& right) {
  return Size(left.width + right.width, left.height + right.height);
}

constexpr Size operator-(const Size& left, const Size& right) {
  return Size(left.width - right.width, left.height - right.height);
}

constexpr bool operator==(const Size& left, const Size& right) {
  return left.width == right.width && left.height == right.height;
}

struct Thickness final {
  constexpr Thickness() : Thickness(0) {}

  constexpr explicit Thickness(const float width)
      : left(width), top(width), right(width), bottom(width) {}

  constexpr explicit Thickness(const float horizontal, const float vertical)
      : left(horizontal), top(vertical), right(horizontal), bottom(vertical) {}

  constexpr Thickness(const float left, const float top, const float right,
                      const float bottom)
      : left(left), top(top), right(right), bottom(bottom) {}

  constexpr float GetHorizontalTotal() const { return left + right; }

  constexpr float GetVerticalTotal() const { return top + bottom; }

  void SetLeftRight(const float value) { left = right = value; }

  void SetTopBottom(const float value) { top = bottom = value; }

  void SetAll(const float value) { left = top = right = bottom = value; }

  constexpr float Validate() const {
    return left >= 0.0 && top >= 0.0 && right >= 0.0 && bottom >= 0.0;
  }

  float left;
  float top;
  float right;
  float bottom;
};

constexpr Size operator+(const Size& size, const Thickness& thickness) {
  return {size.width + thickness.left + thickness.right,
          size.height + thickness.top + thickness.bottom};
}

constexpr Size operator+(const Thickness& thickness, const Size& size) {
  return operator+(size, thickness);
}

constexpr Thickness operator+(const Thickness& left, const Thickness& right) {
  return {left.left + right.left, left.top + right.top,
          left.right + right.right, left.bottom + right.bottom};
}

constexpr bool operator==(const Thickness& left, const Thickness& right) {
  return left.left == right.left && left.top == right.top &&
         left.right == right.right && left.bottom == right.bottom;
}

struct Rect final {
  constexpr Rect() = default;
  constexpr Rect(const float left, const float top, const float width,
                 const float height)
      : left(left), top(top), width(width), height(height) {}
  constexpr Rect(const Point& lefttop, const Size& size)
      : left(lefttop.x),
        top(lefttop.y),
        width(size.width),
        height(size.height) {}

  constexpr static Rect FromVertices(const float left, const float top,
                                     const float right, const float bottom) {
    return Rect(left, top, right - left, bottom - top);
  }

  constexpr static Rect FromCenter(const Point& center, const float width,
                                   const float height) {
    return Rect(center.x - width / 2.0f, center.y - height / 2.0f, width,
                height);
  }

  constexpr float GetRight() const { return left + width; }

  constexpr float GetBottom() const { return top + height; }

  constexpr Point GetLeftTop() const { return Point(left, top); }

  constexpr Point GetRightBottom() const {
    return Point(left + width, top + height);
  }

  constexpr Point GetLeftBottom() const { return Point(left, top + height); }

  constexpr Point GetRightTop() const { return Point(left + width, top); }

  constexpr Point GetCenter() const {
    return Point(left + width / 2.0f, top + height / 2.0f);
  }

  constexpr void SetSize(const Size& size) {
    width = size.width;
    height = size.height;
  }

  constexpr Size GetSize() const { return Size(width, height); }

  constexpr Rect Expand(const Thickness& thickness) const {
    return Rect(left - thickness.left, top - thickness.top,
                width + thickness.GetHorizontalTotal(),
                height + thickness.GetVerticalTotal());
  }

  constexpr Rect Shrink(const Thickness& thickness,
                        bool normalize = false) const {
    Rect result(left + thickness.left, top + thickness.top,
                width - thickness.GetHorizontalTotal(),
                height - thickness.GetVerticalTotal());

    if (normalize) {
      if (result.width < 0) {
        result.width = 0;
      }
      if (result.height < 0) {
        result.height = 0;
      }
    }

    return result;
  }

  constexpr bool IsPointInside(const Point& point) const {
    return point.x >= left && point.x < GetRight() && point.y >= top &&
           point.y < GetBottom();
  }

  constexpr Rect Normalize() const {
    Rect result = *this;
    if (result.width < 0) {
      result.left += result.width;
      result.width = -result.width;
    }
    if (result.height < 0) {
      result.top += result.height;
      result.height = -result.height;
    }
    return result;
  }

  std::string ToString() const {
    return std::format("Rect(left: {}, top: {}, width: {}, height: {})", left,
                       top, width, height);
  }

  float left = 0.0f;
  float top = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

constexpr bool operator==(const Rect& left, const Rect& right) {
  return left.left == right.left && left.top == right.top &&
         left.width == right.width && left.height == right.height;
}

struct RoundedRect final {
  constexpr RoundedRect() = default;
  constexpr RoundedRect(const Rect& rect, const float radius_x,
                        const float radius_y)
      : rect(rect), radius_x(radius_x), radius_y(radius_y) {}

  Rect rect{};
  float radius_x = 0.0f;
  float radius_y = 0.0f;
};

constexpr bool operator==(const RoundedRect& left, const RoundedRect& right) {
  return left.rect == right.rect && left.radius_x == right.radius_x &&
         left.radius_y == right.radius_y;
}

struct Ellipse final {
  constexpr Ellipse() = default;
  constexpr Ellipse(const Point& center, const float radius_x,
                    const float radius_y)
      : center(center), radius_x(radius_x), radius_y(radius_y) {}

  constexpr static Ellipse FromRect(const Rect& rect) {
    return Ellipse(rect.GetCenter(), rect.width / 2.0f, rect.height / 2.0f);
  }

  constexpr Rect GetBoundRect() const {
    return Rect::FromCenter(center, radius_x * 2.0f, radius_y * 2.0f);
  }

  Point center{};
  float radius_x = 0.0f;
  float radius_y = 0.0f;
};

constexpr bool operator==(const Ellipse& left, const Ellipse& right) {
  return left.center == right.center && left.radius_x == right.radius_x &&
         left.radius_y == right.radius_y;
}

using TextRange = Range;
}  // namespace cru::platform

template <>
struct std::formatter<cru::platform::Point, char>
    : cru::string::ImplementFormatterByToString<cru::platform::Point> {};

template <>
struct std::formatter<cru::platform::Size, char>
    : cru::string::ImplementFormatterByToString<cru::platform::Size> {};

template <>
struct std::formatter<cru::platform::Rect, char>
    : cru::string::ImplementFormatterByToString<cru::platform::Rect> {};
