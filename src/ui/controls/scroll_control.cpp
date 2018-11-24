#include "scroll_control.hpp"

#include <limits>

#include "cru_debug.hpp"
#include "format.hpp"

namespace cru::ui::controls
{
    ScrollControl::ScrollControl(const bool container) : Control(container)
    {
        SetClipToPadding(true);
    }

    ScrollControl::~ScrollControl()
    {

    }

    void ScrollControl::SetHorizontalScrollEnabled(const bool enable)
    {
        horizontal_scroll_enabled_ = enable;
        InvalidateLayout();
        InvalidateDraw();
    }

    void ScrollControl::SetVerticalScrollEnabled(const bool enable)
    {
        vertical_scroll_enabled_ = enable;
        InvalidateLayout();
        InvalidateDraw();
    }

    Control* ScrollControl::HitTest(const Point& point)
    {

    }

    void ScrollControl::SetViewWidth(const float length)
    {
        view_width_ = length;
    }

    void ScrollControl::SetViewHeight(const float length)
    {
        view_height_ = length;
    }

    Size ScrollControl::OnMeasureContent(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        auto available_size_for_children = available_size;
        if (IsHorizontalScrollEnabled())
        {
            if (layout_params->width.mode == MeasureMode::Content)
                debug::DebugMessage(L"ScrollView: Width measure mode is Content and horizontal scroll is enabled. So Stretch is used instead.");

            for (auto child : GetChildren())
            {
                const auto child_layout_params = child->GetLayoutParams();
                if (child_layout_params->width.mode == MeasureMode::Stretch)
                    throw std::runtime_error(Format("ScrollView: Horizontal scroll is enabled but a child {} 's width measure mode is Stretch which may cause infinite length.", ToUtf8String(child->GetControlType())));
            }

            available_size_for_children.width = std::numeric_limits<float>::max();
        }

        if (IsVerticalScrollEnabled())
        {
            if (layout_params->height.mode == MeasureMode::Content)
                debug::DebugMessage(L"ScrollView: Height measure mode is Content and vertical scroll is enabled. So Stretch is used instead.");

            for (auto child : GetChildren())
            {
                const auto child_layout_params = child->GetLayoutParams();
                if (child_layout_params->height.mode == MeasureMode::Stretch)
                    throw std::runtime_error(Format("ScrollView: Vertical scroll is enabled but a child {} 's height measure mode is Stretch which may cause infinite length.", ToUtf8String(child->GetControlType())));
            }

            available_size_for_children.height = std::numeric_limits<float>::max();
        }

        auto max_child_size = Size::Zero();
        for (auto control: GetChildren())
        {
            control->Measure(available_size_for_children);
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
            const auto child_layout_params = control->GetLayoutParams();
            if (child_layout_params->width.mode == MeasureMode::Stretch)
                size.width = max_child_size.width;
            if (child_layout_params->height.mode == MeasureMode::Stretch)
                size.height = max_child_size.height;
            control->SetDesiredSize(size);
        }

        auto result = max_child_size;
        if (IsHorizontalScrollEnabled())
        {
            SetViewWidth(max_child_size.width);
            result.width = available_size.width;
        }
        if (IsVerticalScrollEnabled())
        {
            SetViewHeight(max_child_size.height);
            result.height = available_size.height;
        }

        return result;
    }

    void ScrollControl::OnLayoutContent(const Rect& rect)
    {
        auto layout_rect = rect;

        if (IsHorizontalScrollEnabled())
            layout_rect.width = GetViewWidth();
        if (IsVerticalScrollEnabled())
            layout_rect.height = GetViewHeight();

        for (auto control: GetChildren())
        {
            const auto size = control->GetDesiredSize();
            // Ignore alignment, always center aligned.
            auto&& calculate_anchor = [](const float anchor, const float layout_length, const float control_length) -> float
            {
                return anchor + (layout_length - control_length) / 2;
            };

            control->Layout(Rect(Point(
                calculate_anchor(rect.left, layout_rect.width, size.width),
                calculate_anchor(rect.top, layout_rect.height, size.height)
            ), size));
        }
    }
}
