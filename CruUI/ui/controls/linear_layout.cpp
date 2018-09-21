#include "linear_layout.h"

namespace cru::ui::controls
{
    LinearLayout::LinearLayout(const Orientation orientation)
        : Control(true), orientation_(orientation)
    {

    }

    inline float AtLeast0(const float value)
    {
        return value < 0 ? 0 : value;
    }

    inline Size AtLeast0(const Size& size)
    {
        return Size(AtLeast0(size.width), AtLeast0(size.height));
    }

    Size LinearLayout::OnMeasure(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto&& get_available_length_for_child = [](const LayoutLength& layout_length, const float available_length) -> float
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

        float secondary_side_child_max_length = 0;

        std::list<Control*> stretch_control_list;

        // First measure Content and Exactly and count Stretch.
        if (orientation_ == Orientation::Horizontal)
            ForeachChild([&](Control* const control)
        {
            const auto mode = control->GetLayoutParams()->width.mode;
            if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
            {
                control->Measure(AtLeast0(rest_available_size_for_children));
                const auto size = control->GetDesiredSize();
                rest_available_size_for_children.width -= size.width;
                secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
            }
            else
                stretch_control_list.push_back(control);
        });
        else
            ForeachChild([&](Control* const control)
        {
            const auto mode = control->GetLayoutParams()->height.mode;
            if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
            {
                control->Measure(AtLeast0(rest_available_size_for_children));
                const auto size = control->GetDesiredSize();
                rest_available_size_for_children.height -= size.height;
                secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
            }
            else
                stretch_control_list.push_back(control);
        });

        if (orientation_ == Orientation::Horizontal)
        {
            const auto available_width = rest_available_size_for_children.width / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_width, AtLeast0(rest_available_size_for_children.height)));
                const auto size = control->GetDesiredSize();
                rest_available_size_for_children.width -= size.width;
                secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
            }
        }
        else
        {
            const auto available_height = rest_available_size_for_children.height / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(AtLeast0(rest_available_size_for_children.width), available_height));
                const auto size = control->GetDesiredSize();
                rest_available_size_for_children.height -= size.height;
                secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
            }
        }

        auto actual_size_for_children = total_available_size_for_children;
        if (orientation_ == Orientation::Horizontal)
        {
            actual_size_for_children.width -= rest_available_size_for_children.width;
            actual_size_for_children.height = secondary_side_child_max_length;
        }
        else
        {
            actual_size_for_children.width = secondary_side_child_max_length;
            actual_size_for_children.height -= rest_available_size_for_children.height;
        }

        auto&& calculate_final_length = [](const LayoutLength& layout_length, const float length_for_children, const float max_child_length) -> float
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
        float current_anchor_length = 0;
        ForeachChild([this, &current_anchor_length, rect](Control* control)
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetDesiredSize();
            const auto alignment = orientation_ == Orientation::Horizontal ? layout_params->height.alignment : layout_params->width.alignment;

            auto&& calculate_anchor = [alignment](const float layout_length, const float control_length) -> float
            {
                switch (alignment)
                {
                case Alignment::Center:
                    return (layout_length - control_length) / 2;
                case Alignment::Start:
                    return 0;
                case Alignment::End:
                    return layout_length - control_length;
                default:
                    UnreachableCode();
                }
            };

            if (orientation_ == Orientation::Horizontal)
            {
                control->Layout(Rect(Point(current_anchor_length, calculate_anchor(rect.height, size.height)), size));
                current_anchor_length += size.width;
            }
            else
            {
                control->Layout(Rect(Point(calculate_anchor(rect.width, size.width), current_anchor_length), size));
                current_anchor_length += size.height;
            }
        });
    }
}
