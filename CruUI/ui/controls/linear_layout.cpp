#include "linear_layout.h"

namespace cru::ui::controls
{
    LinearLayout::LinearLayout(const Orientation orientation)
        : Control(true), orientation_(orientation)
    {

    }

    Size LinearLayout::OnMeasure(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto&& get_available_length_for_child = [](const MeasureLength& layout_length, const float available_length) -> float
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

        Size total_available_size_for_children(
            get_available_length_for_child(layout_params->width, available_size.width),
            get_available_length_for_child(layout_params->height, available_size.height)
        );

        auto rest_available_size_for_children = total_available_size_for_children;

        ForeachChild([this, &rest_available_size_for_children](Control* const control)
        {
            control->Measure(rest_available_size_for_children);
            if (orientation_ == Orientation::Horizontal)
            {
                rest_available_size_for_children.width -= control->GetDesiredSize().width;
                if (rest_available_size_for_children.width < 0)
                    rest_available_size_for_children.width = 0;
            }
            else
            {
                rest_available_size_for_children.height -= control->GetDesiredSize().height;
                if (rest_available_size_for_children.height < 0)
                    rest_available_size_for_children.height = 0;
            }
        });

        auto actual_size_for_children = total_available_size_for_children;
        if (orientation_ == Orientation::Horizontal)
            actual_size_for_children.width -= rest_available_size_for_children.width;
        else
            actual_size_for_children.height -= rest_available_size_for_children.height;

        auto&& calculate_final_length = [](const MeasureLength& layout_length, const float length_for_children, const float max_child_length) -> float
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
            calculate_final_length(layout_params->width, total_available_size_for_children.width, actual_size_for_children.width),
            calculate_final_length(layout_params->height, total_available_size_for_children.height, actual_size_for_children.height)
        );
    }

    void LinearLayout::OnLayout(const Rect& rect)
    {
        auto current_anchor = Point::zero;
        ForeachChild([this, &current_anchor](Control* control)
        {
            const auto size = control->GetDesiredSize();
            control->Layout(Rect(current_anchor, size));

            if (orientation_ == Orientation::Horizontal)
                current_anchor.x += size.width;
            else
                current_anchor.y += size.height;
        });
    }
}
