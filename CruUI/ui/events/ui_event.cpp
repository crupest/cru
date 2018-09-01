#include "ui_event.h"

#include "ui/control.h"

namespace cru
{
    namespace ui
    {
        namespace events
        {
            Point MouseEventArgs::GetPoint(Control* control) const
            {
                if (point_.has_value())
                    return control->AbsoluteToLocal(point_.value());
                return Point();
            }
        }
    }
}
