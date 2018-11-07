#include "ui_event.hpp"

#include "ui/control.hpp"

namespace cru::ui::events
{
    Point MouseEventArgs::GetPoint(Control* control, const RectRange range) const
    {
        if (point_.has_value())
            return control->TransformPoint(control->WindowToControl(point_.value()), RectRange::Margin, range);
        return Point();
    }
}
