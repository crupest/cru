#pragma once

#include "system_headers.hpp"

#include "ui_base.hpp"

namespace cru::ui
{
    inline D2D1_POINT_2F Convert(const Point& point)
    {
        return D2D1::Point2F(point.x, point.y);
    }

    inline D2D1_RECT_F Convert(const Rect& rect)
    {
        return D2D1::RectF(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height);
    }
}
