#include "frame_layout.hpp"

namespace cru::ui::controls
{
    FrameLayout::FrameLayout() = default;

    FrameLayout::~FrameLayout() = default;

    StringView FrameLayout::GetControlType() const
    {
        return control_type;
    }

    Size FrameLayout::OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        auto max_child_size = Size::Zero();
        for (auto control: GetChildren())
        {
            control->Measure(available_size, additional_info);
            const auto&& size = control->GetDesiredSize();
            if (max_child_size.width < size.width)
                max_child_size.width = size.width;
            if (max_child_size.height < size.height)
                max_child_size.height = size.height;
        }

        // coerce size fro stretch.
        for (auto control: GetChildren())
        {
            auto size = control->GetDesiredSize();
            const auto layout_params = control->GetLayoutParams();
            if (layout_params->width.mode == MeasureMode::Stretch)
                size.width = max_child_size.width;
            if (layout_params->height.mode == MeasureMode::Stretch)
                size.height = max_child_size.height;
            control->SetDesiredSize(size);
        }

        return max_child_size;
    }

    void FrameLayout::OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {
        for (auto control: GetChildren())
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
                calculate_anchor(rect.left, layout_params->width.alignment, rect.width, size.width),
                calculate_anchor(rect.top, layout_params->height.alignment, rect.height, size.height)
            ), size), additional_info);
        }
    }
}
