#pragma once

#include "system_headers.hpp"

#include "ui_base.hpp"

namespace cru::ui
{
    inline D2D1_RECT_F Convert(const Rect& rect)
    {
        return D2D1::RectF(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height);
    }
}
