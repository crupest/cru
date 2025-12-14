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

  constexpr bool operator==(const Point& other) const = default;

  float x = 0;
  float y = 0;
};

constexpr Point operator+(const Point& left, const Point& right) {
  return Point(left.x + right.x, left.y + right.y);
}

constexpr Point operator-(const Point& left, const Point& right) {
  return Point(left.x - right.x, left.y - right.y);
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

  constexpr Size Min(const Size& other) const {
    return {std::min(width, other.width), std::min(height, other.height)};
  }

  constexpr Size Max(const Size& other) const {
    return {std::max(width, other.width), std::max(height, other.height)};
  }

  constexpr Size AtLeast0() const { return Max(kZero); }

  std::string ToString() const {
    return std::format("Size(width: {}, height: {})", width, height);
  }

  constexpr bool operator==(const Size& other) const = default;

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

  constexpr Size GetTotalSize() const {
    return {GetHorizontalTotal(), GetVerticalTotal()};
  }

  void SetLeftRight(const float value) { left = right = value; }

  void SetTopBottom(const float value) { top = bottom = value; }

  void SetAll(const float value) { left = top = right = bottom = value; }

  constexpr bool operator==(const Thickness& other) const = default;

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

constexpr Thickness operator*(const Thickness& thickness, float scale) {
  return {thickness.left * scale, thickness.top * scale,
          thickness.right * scale, thickness.bottom * scale};
}

constexpr Thickness operator*(float scale, const Thickness& thickness) {
  return operator*(thickness, scale);
}

constexpr Thickness operator/(const Thickness& thickness, float scale) {
  return {thickness.left / scale, thickness.top / scale,
          thickness.right / scale, thickness.bottom / scale};
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
    return {left, top, right - left, bottom - top};
  }

  constexpr static Rect FromVertices(const Point& lefttop,
                                     const Point& rightbottom) {
    return {lefttop.x, lefttop.y, rightbottom.x - lefttop.x,
            rightbottom.y - lefttop.y};
  }

  constexpr static Rect FromCenter(const Point& center, const float width,
                                   const float height) {
    return {center.x - width / 2.0f, center.y - height / 2.0f, width, height};
  }

  constexpr bool HasNoSize() const { return width == 0.f || height == 0.f; }

  constexpr float GetRight() const { return left + width; }

  constexpr float GetBottom() const { return top + height; }

  constexpr Point GetLeftTop() const { return {left, top}; }

  constexpr Point GetRightBottom() const {
    return {left + width, top + height};
  }

  constexpr Point GetLeftBottom() const { return {left, top + height}; }

  constexpr Point GetRightTop() const { return {left + width, top}; }

  constexpr Point GetCenter() const {
    return {left + width / 2.0f, top + height / 2.0f};
  }

  constexpr void SetSize(const Size& size) {
    width = size.width;
    height = size.height;
  }

  constexpr Size GetSize() const { return Size(width, height); }

  constexpr Rect WithOffset(const Point& offset) const {
    return {left + offset.x, top + offset.y, width, height};
  }

  constexpr Rect Expand(const Thickness& thickness) const {
    return {left - thickness.left, top - thickness.top,
            width + thickness.GetHorizontalTotal(),
            height + thickness.GetVerticalTotal()};
  }

  constexpr Rect Shrink(const Thickness& thickness,
                        bool normalize = false) const {
    return {left + thickness.left, top + thickness.top,
            width - thickness.GetHorizontalTotal(),
            height - thickness.GetVerticalTotal()};
  }

  constexpr Rect Scale(float scale) const {
    return {left * scale, top * scale, width * scale, height * scale};
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

  constexpr bool IsIntersect(const Rect& other) const {
    if (HasNoSize() || other.HasNoSize()) {
      return false;
    }

    if (left >= other.GetRight() || GetRight() <= other.left) {
      return false;
    }

    if (top >= other.GetBottom() || GetBottom() <= other.top) {
      return false;
    }

    return true;
  }

  constexpr Rect Union(const Rect& other) const {
    return FromVertices(std::min(left, other.left), std::min(top, other.top),
                        std::max(GetRight(), other.GetRight()),
                        std::max(GetBottom(), other.GetBottom()));
  }

  std::string ToString() const {
    return std::format("Rect(left: {}, top: {}, width: {}, height: {})", left,
                       top, width, height);
  }

  constexpr bool operator==(const Rect& other) const = default;

  float left = 0.0f;
  float top = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

struct RoundedRect final {
  constexpr RoundedRect() = default;
  constexpr RoundedRect(const Rect& rect, const float radius_x,
                        const float radius_y)
      : rect(rect), radius_x(radius_x), radius_y(radius_y) {}

  constexpr bool operator==(const RoundedRect& other) const = default;

  Rect rect{};
  float radius_x = 0.0f;
  float radius_y = 0.0f;
};

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

  constexpr bool operator==(const Ellipse& other) const = default;

  Point center{};
  float radius_x = 0.0f;
  float radius_y = 0.0f;
};

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
