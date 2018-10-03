#pragma once


namespace cru
{
    namespace ui
    {
        struct Point
        {
            constexpr static Point Zero()
            {
                return Point(0, 0);
            }

            constexpr Point() = default;
            constexpr Point(const float x, const float y) : x(x), y(y) { }

            float x = 0;
            float y = 0;
        };

        constexpr bool operator==(const Point& left, const Point& right)
        {
            return left.x == right.x && left.y == right.y;
        }

        constexpr bool operator!=(const Point& left, const Point& right)
        {
            return !(left == right);
        }

        struct Size
        {
            constexpr static Size Zero()
            {
                return Size(0, 0);
            }

            constexpr Size() = default;
            constexpr Size(const float width, const float height) : width(width), height(height) { }

            float width = 0;
            float height = 0;
        };

        constexpr Size operator + (const Size& left, const Size& right)
        {
            return Size(left.width + right.width, left.height + right.height);
        }

        constexpr Size operator - (const Size& left, const Size& right)
        {
            return Size(left.width - right.width, left.height - right.height);
        }

        constexpr bool operator==(const Size& left, const Size& right)
        {
            return left.width == right.width && left.height == right.height;
        }

        constexpr bool operator!=(const Size& left, const Size& right)
        {
            return !(left == right);
        }

        struct Rect
        {
            constexpr Rect() = default;
            constexpr Rect(const float left, const float top, const float width, const float height)
                : left(left), top(top), width(width), height(height) { }
            constexpr Rect(const Point& lefttop, const Size& size)
                : left(lefttop.x), top(lefttop.y), width(size.width), height(size.height) { }

            constexpr static Rect FromVertices(const float left, const float top, const float right, const float bottom)
            {
                return Rect(left, top, right - left, bottom - top);
            }

            constexpr float GetRight() const
            {
                return left + width;
            }

            constexpr float GetBottom() const
            {
                return top + height;
            }

            constexpr Point GetLeftTop() const
            {
                return Point(left, top);
            }

            constexpr Point GetRightBottom() const
            {
                return Point(left + width, top + height);
            }

            constexpr Size GetSize() const
            {
                return Size(width, height);
            }

            constexpr bool IsPointInside(const Point& point) const
            {
                return
                    point.x >= left &&
                    point.x < GetRight() &&
                    point.y >= top &&
                    point.y < GetBottom();
            }

            float left = 0.0f;
            float top = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
        };

        enum class MouseButton
        {
            Left,
            Right,
            Middle
        };

        struct TextRange
        {
            constexpr TextRange() = default;
            constexpr TextRange(const int position, const int count)
                : position(position), count(count)
            {

            }

            unsigned position = 0;
            unsigned count = 0;
        };
    }
}
