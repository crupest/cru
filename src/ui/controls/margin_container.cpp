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
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto&& get_available_length_for_child = [](const LayoutSideParams& layout_length, const float available_length) -> float
        {
            switch (layout_length.mode)
            {
            case MeasureMode::Exactly:
            {
                return std::min(layout_length.length, available_length);
            }
            case MeasureMode::Stretch:
            case MeasureMode::Content:
            {
                return available_length;
            }
            default:
                UnreachableCode();
            }
        };

        const Size size_for_children(get_available_length_for_child(layout_params->width, available_size.width),
            get_available_length_for_child(layout_params->height, available_size.height));

        const auto margin_size = Size(margin_.left + margin_.right, margin_.top + margin_.bottom);
        const auto coerced_size_for_children = AtLeast0(size_for_children - margin_size);

        auto max_child_size = Size::Zero();
        ForeachChild([coerced_size_for_children, &max_child_size](Control* control)
        {
            control->Measure(coerced_size_for_children);
            const auto&& size = control->GetDesiredSize();
            if (max_child_size.width < size.width)
                max_child_size.width = size.width;
            if (max_child_size.height < size.height)
                max_child_size.height = size.height;
        });

        auto&& calculate_final_length = [](const LayoutSideParams& layout_length, const float length_for_children, const float max_child_length) -> float
        {
            switch (layout_length.mode)
            {
            case MeasureMode::Exactly:
            case MeasureMode::Stretch:
                return length_for_children;
            case MeasureMode::Content:
                return max_child_length;
            default:
                UnreachableCode();
            }
        };

        return Size(
            calculate_final_length(layout_params->width, coerced_size_for_children.width, max_child_size.width),
            calculate_final_length(layout_params->height, coerced_size_for_children.height, max_child_size.height)
        );
    }

    void MarginContainer::OnLayout(const Rect& rect)
    {
        const auto anchor = Point(margin_.left, margin_.top);
        const auto margin_size = Size(margin_.left + margin_.right, margin_.top + margin_.bottom);
        Control::OnLayout(Rect(anchor, AtLeast0(rect.GetSize() - margin_size)));
    }
}
