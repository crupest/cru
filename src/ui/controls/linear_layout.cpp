#include "linear_layout.hpp"

#include <algorithm>

#include "math_util.hpp"

namespace cru::ui::controls
{
    LinearLayout::LinearLayout(const Orientation orientation)
        : Control(true), orientation_(orientation)
    {

    }

    StringView LinearLayout::GetControlType() const
    {
        return control_type;
    }

    Size LinearLayout::OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        auto actual_size_for_children = Size::Zero();

        float secondary_side_child_max_length = 0;

        std::list<Control*> stretch_control_list;

        // First measure Content and Exactly and count Stretch.
        if (orientation_ == Orientation::Horizontal)
            for(auto control: GetChildren())
            {
                const auto mode = control->GetLayoutParams()->width.mode;
                if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
                {
                    Size current_available_size(AtLeast0(available_size.width - actual_size_for_children.width), available_size.height);
                    control->Measure(current_available_size, additional_info);
                    const auto size = control->GetDesiredSize();
                    actual_size_for_children.width += size.width;
                    secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
                }
                else
                    stretch_control_list.push_back(control);
            }
        else
            for(auto control: GetChildren())
            {
                const auto mode = control->GetLayoutParams()->height.mode;
                if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
                {
                    Size current_available_size(available_size.width, AtLeast0(available_size.height - actual_size_for_children.height));
                    control->Measure(current_available_size, additional_info);
                    const auto size = control->GetDesiredSize();
                    actual_size_for_children.height += size.height;
                    secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
                }
                else
                    stretch_control_list.push_back(control);
            }

        if (orientation_ == Orientation::Horizontal)
        {
            const auto available_width = AtLeast0(available_size.width - actual_size_for_children.width) / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_width, available_size.height), additional_info);
                const auto size = control->GetDesiredSize();
                actual_size_for_children.width += size.width;
                secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
            }
        }
        else
        {
            const auto available_height = AtLeast0(available_size.height - actual_size_for_children.height) / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_size.width, available_height), additional_info);
                const auto size = control->GetDesiredSize();
                actual_size_for_children.height += size.height;
                secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
            }
        }

        if (orientation_ == Orientation::Horizontal)
        {
            for (auto control : GetChildren())
            {
                if (control->GetLayoutParams()->height.mode == MeasureMode::Stretch)
                {
                    control->SetDesiredSize(Size(control->GetDesiredSize().width, secondary_side_child_max_length));
                }
            }
            actual_size_for_children.height = secondary_side_child_max_length;
        }
        else
        {
            for (auto control : GetChildren())
            {
                if (control->GetLayoutParams()->width.mode == MeasureMode::Stretch)
                {
                    control->SetDesiredSize(Size(secondary_side_child_max_length, control->GetDesiredSize().height));
                }
            }

            actual_size_for_children.width = secondary_side_child_max_length;
        }

        return actual_size_for_children;
    }

    void LinearLayout::OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {
        float current_main_side_anchor = 0;
        for(auto control: GetChildren())
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetDesiredSize();
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
                control->Layout(calculate_rect(current_main_side_anchor, calculate_secondary_side_anchor(rect.height, size.height)), additional_info);
                current_main_side_anchor += size.width;
            }
            else
            {
                control->Layout(calculate_rect(calculate_secondary_side_anchor(rect.width, size.width), current_main_side_anchor), additional_info);
                current_main_side_anchor += size.height;
            }
        }
    }
}
