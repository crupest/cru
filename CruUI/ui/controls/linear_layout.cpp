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

        std::list<Control*> stretch_control_list;

        // First measure Content and Exactly and count Stretch.
        if (orientation_ == Orientation::Horizontal)
            ForeachChild([&rest_available_size_for_children, &stretch_control_list](Control* const control)
        {
            const auto mode = control->GetLayoutParams()->width.mode;
            if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
            {
                control->Measure(rest_available_size_for_children);
                rest_available_size_for_children.width -= control->GetDesiredSize().width;
                if (rest_available_size_for_children.width < 0)
                    rest_available_size_for_children.width = 0;
            }
            else
                stretch_control_list.push_back(control);
        });
        else
            ForeachChild([&rest_available_size_for_children, &stretch_control_list](Control* const control)
        {
            const auto mode = control->GetLayoutParams()->height.mode;
            if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
            {
                control->Measure(rest_available_size_for_children);
                rest_available_size_for_children.height -= control->GetDesiredSize().height;
                if (rest_available_size_for_children.height < 0)
                    rest_available_size_for_children.height = 0;
            }
            else
                stretch_control_list.push_back(control);
        });

        if (orientation_ == Orientation::Horizontal)
        {
            const auto available_width = rest_available_size_for_children.width / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_width, rest_available_size_for_children.height));
                rest_available_size_for_children.width -= control->GetDesiredSize().width;
                if (rest_available_size_for_children.width < 0)
                    rest_available_size_for_children.width = 0;
            }
        }
        else
        {
            const auto available_height = rest_available_size_for_children.height / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(rest_available_size_for_children.width, available_height));
                rest_available_size_for_children.height -= control->GetDesiredSize().height;
                if (rest_available_size_for_children.height < 0)
                    rest_available_size_for_children.height = 0;
            }
        }

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
        float current_anchor_length = 0;
        ForeachChild([this, &current_anchor_length, rect](Control* control)
        {
            const auto size = control->GetDesiredSize();
            if (orientation_ == Orientation::Horizontal)
            {
                control->Layout(Rect(Point(current_anchor_length, (rect.height - size.height) / 2), size));
                current_anchor_length += size.width;
            }
            else
            {
                control->Layout(Rect(Point((rect.width - size.width) / 2, current_anchor_length), size));
                current_anchor_length += size.height;
            }
        });
    }
}
