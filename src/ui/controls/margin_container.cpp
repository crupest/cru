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
        const auto margin_size = Size(margin_.left + margin_.right, margin_.top + margin_.bottom);
        const auto coerced_available_size = AtLeast0(available_size - margin_size);
        return Control::OnMeasure(coerced_available_size) + margin_size;
    }

    void MarginContainer::OnLayout(const Rect& rect)
    {
        const auto anchor = Point(margin_.left, margin_.top);
        const auto margin_size = Size(margin_.left + margin_.right, margin_.top + margin_.bottom);
        ForeachChild([anchor, margin_size, rect](Control* control)
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetDesiredSize();

            auto&& calculate_anchor = [](const float anchor, const Alignment alignment, const float layout_length, const float control_length) -> float
            {
                switch (alignment)
                {
                case Alignment::Center:
                    return anchor + (layout_length - control_length) / 2;
                case Alignment::Start:
                    return anchor;
                case Alignment::End:
                    return anchor + layout_length - control_length;
                default:
                    UnreachableCode();
                }
            };

            control->Layout(Rect(Point(
                calculate_anchor(anchor.x, layout_params->width.alignment, rect.width - margin_size.width, size.width),
                calculate_anchor(anchor.y, layout_params->height.alignment, rect.height - margin_size.height, size.height)
            ), size));
        });
    }
}
