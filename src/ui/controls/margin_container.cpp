#include "margin_container.h"

namespace cru::ui::controls
{
    inline float AtLeast0(const float value)
    {
        return value < 0 ? 0 : value;
    }

    inline Size AtLeast0(const Size& size)
    {
        return Size(AtLeast0(size.width), AtLeast0(size.height));
    }

    MarginContainer::MarginContainer(const Thickness& margin)
        : Control(true), margin_(margin)
    {
    }

    void MarginContainer::SetMargin(const Thickness& margin)
    {
        margin_ = margin;
        InvalidateLayout();
    }

    Size MarginContainer::OnMeasure(const Size& available_size)
    {
        return DefaultMeasureWithPadding(available_size, margin_);
    }

    void MarginContainer::OnLayout(const Rect& rect)
    {
        DefaultLayoutWithPadding(rect, margin_);
    }
}
