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

    Size LinearLayout::OnMeasureContent(const Size& available_size)
    {
        auto rest_available_size_for_children = available_size;

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
                control->Measure(Size(AtLeast0(available_width), rest_available_size_for_children.height));
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
                control->Measure(Size(rest_available_size_for_children.width, AtLeast0(available_height)));
                const auto size = control->GetDesiredSize();
                rest_available_size_for_children.height -= size.height;
                secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
            }
        }

        auto actual_size_for_children = available_size;
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

        return actual_size_for_children;
    }

    void LinearLayout::OnLayoutContent(const Rect& rect)
    {
        float current_main_side_anchor = 0;
        ForeachChild([this, &current_main_side_anchor, rect](Control* control)
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetSize();
            const auto alignment = orientation_ == Orientation::Horizontal ? layout_params->height.alignment : layout_params->width.alignment;

            auto&& calculate_secondary_side_anchor = [alignment](const float layout_length, const float control_length) -> float
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

            auto&& calculate_rect = [rect, size](const float anchor_left, const float anchor_top)
            {
                return Rect(Point(rect.left + anchor_left, rect.top + anchor_top), size);
            };

            if (orientation_ == Orientation::Horizontal)
            {
                control->Layout(calculate_rect(current_main_side_anchor, calculate_secondary_side_anchor(rect.height, size.height)));
                current_main_side_anchor += size.width;
            }
            else
            {
                control->Layout(calculate_rect(calculate_secondary_side_anchor(rect.width, size.width), current_main_side_anchor));
                current_main_side_anchor += size.height;
            }
        });
    }
}
