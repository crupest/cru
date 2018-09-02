#pragma once

namespace cru
{
    namespace ui
    {
        struct Point
        {
            static const Point zero;

            Point() = default;
            Point(const float x, const float y) : x(x), y(y) { }

            float x;
            float y;
        };

        inline bool operator==(const Point& left, const Point& right)
        {
            return left.x == right.x && left.y == right.y;
        }

        inline bool operator!=(const Point& left, const Point& right)
        {
            return !(left == right);
        }

        struct Size
        {
            static const Size zero;

            Size() = default;
            Size(const float width, const float height) : width(width), height(height) { }

            float width;
            float height;
        };

        struct Rect
        {
            Rect() = default;
            Rect(const float left, const float top, const float width, const float height)
                : left(left), top(top), width(width), height(height) { }
            Rect(const Point& lefttop, const Size& size)
                : left(lefttop.x), top(lefttop.y), width(size.width), height(size.height) { }

            static Rect FromVertices(const float left, const float top, const float right, const float bottom)
            {
                return Rect(left, top, right - left, bottom - top);
            }

            float GetRight() const
            {
                return left + width;
            }

            float GetBottom() const
            {
                return top + height;
            }

            Point GetLeftTop() const
            {
                return Point(left, top);
            }

            Point GetRightBottom() const
            {
                return Point(left + width, top + height);
            }

            Size GetSize() const
            {
                return Size(width, height);
            }

            bool IsPointInside(const Point& point) const
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

        struct Thickness
        {
            Thickness() : Thickness(0) { }
            explicit Thickness(const float width)
                : left(width), top(width), right(width), bottom(width) { }

            Thickness(const float left, const float top, const float right, const float bottom)
                : left(left), top(top), right(right), bottom(bottom) { }


            float left;
            float top;
            float right;
            float bottom;
        };

        enum class MouseButton
        {
            Left,
            Right,
            Middle
        };
    }
}
