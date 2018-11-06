#include "ui_event.h"

#include "ui/control.h"

namespace cru
{
    namespace ui
    {
        namespace events
        {
            Point MouseEventArgs::GetPoint(Control* control, const RectRange range) const
            {
                if (point_.has_value())
                    return control->TransformPoint(control->AbsoluteToLocal(point_.value()), RectRange::Margin, range);
                return Point();
            }
        }
    }
}
