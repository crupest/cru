#include "scroll_control.hpp"

#include <limits>

#include "cru_debug.hpp"
#include "ui/convert_util.hpp"
#include "exception.hpp"
#include "math_util.hpp"
#include "ui/ui_manager.hpp"
#include "ui/window.hpp"

namespace cru::ui::controls
{
    constexpr auto scroll_bar_width = 15.0f;

    ScrollControl::ScrollControl(const bool container)
    {
        SetClipContent(true);

        draw_foreground_event.AddHandler([this](events::DrawEventArgs& args)
        {
            const auto device_context = args.GetDeviceContext();
            const auto predefined = UiManager::GetInstance()->GetPredefineResources();

            if (is_horizontal_scroll_bar_visible_)
            {
                device_context->FillRectangle(
                    Convert(horizontal_bar_info_.border),
                    predefined->scroll_bar_background_brush.Get()
                );

                device_context->FillRectangle(
                    Convert(horizontal_bar_info_.bar),
                    predefined->scroll_bar_brush.Get()
                );

                device_context->DrawLine(
                    Convert(horizontal_bar_info_.border.GetLeftTop()),
                    Convert(horizontal_bar_info_.border.GetRightTop()),
                    predefined->scroll_bar_border_brush.Get()
                );
            }

            if (is_vertical_scroll_bar_visible_)
            {
                device_context->FillRectangle(
                    Convert(vertical_bar_info_.border),
                    predefined->scroll_bar_background_brush.Get()
                );

                device_context->FillRectangle(
                    Convert(vertical_bar_info_.bar),
                    predefined->scroll_bar_brush.Get()
                );

                device_context->DrawLine(
                    Convert(vertical_bar_info_.border.GetLeftTop()),
                    Convert(vertical_bar_info_.border.GetLeftBottom()),
                    predefined->scroll_bar_border_brush.Get()
                );
            }
        });

        mouse_down_event.tunnel.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetMouseButton() == MouseButton::Left)
            {
                const auto point = args.GetPoint(this);
                if (is_vertical_scroll_bar_visible_ && vertical_bar_info_.bar.IsPointInside(point))
                {
                    GetWindow()->CaptureMouseFor(this);
                    is_pressing_scroll_bar_ = Orientation::Vertical;
                    pressing_delta_ = point.y - vertical_bar_info_.bar.top;
                    args.SetHandled();
                    return;
                }

                if (is_horizontal_scroll_bar_visible_ && horizontal_bar_info_.bar.IsPointInside(point))
                {
                    GetWindow()->CaptureMouseFor(this);
                    pressing_delta_ = point.x - horizontal_bar_info_.bar.left;
                    is_pressing_scroll_bar_ = Orientation::Horizontal;
                    args.SetHandled();
                    return;
                }
            }
        });

        mouse_move_event.tunnel.AddHandler([this](events::MouseEventArgs& args)
        {
            const auto mouse_point = args.GetPoint(this);

            if (is_pressing_scroll_bar_ == Orientation::Horizontal)
            {
                const auto new_head_position = mouse_point.x - pressing_delta_;
                const auto new_offset = new_head_position / horizontal_bar_info_.border.width * view_width_;
                SetScrollOffset(new_offset, std::nullopt);
                args.SetHandled();
                return;
            }

            if (is_pressing_scroll_bar_ == Orientation::Vertical)
            {
                const auto new_head_position = mouse_point.y - pressing_delta_;
                const auto new_offset = new_head_position / vertical_bar_info_.border.height * view_height_;
                SetScrollOffset(std::nullopt, new_offset);
                args.SetHandled();
                return;
            }
        });

        mouse_up_event.tunnel.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetMouseButton() == MouseButton::Left && is_pressing_scroll_bar_.has_value())
            {
                GetWindow()->ReleaseCurrentMouseCapture();
                is_pressing_scroll_bar_ = std::nullopt;
                args.SetHandled();
            }
        });

        mouse_wheel_event.bubble.AddHandler([this](events::MouseWheelEventArgs& args)
        {
            constexpr const auto view_delta = 30.0f;

            if (args.GetDelta() == 0.0f)
                return;

            const auto content_rect = GetRect(RectRange::Content);
            if (IsVerticalScrollEnabled() && GetScrollOffsetY() != (args.GetDelta() > 0.0f ? 0.0f : AtLeast0(GetViewHeight() - content_rect.height)))
            {
                SetScrollOffset(std::nullopt, GetScrollOffsetY() - args.GetDelta() / WHEEL_DELTA * view_delta);
                args.SetHandled();
                return;
            }

            if (IsHorizontalScrollEnabled() && GetScrollOffsetX() != (args.GetDelta() > 0.0f ? 0.0f : AtLeast0(GetViewWidth() - content_rect.width)))
            {
                SetScrollOffset(GetScrollOffsetX() - args.GetDelta() / WHEEL_DELTA * view_delta, std::nullopt);
                args.SetHandled();
                return;
            }
        });
    }

    ScrollControl::~ScrollControl()
    {

    }

    StringView ScrollControl::GetControlType() const
    {
        return control_type;
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

    void ScrollControl::SetHorizontalScrollBarVisibility(const ScrollBarVisibility visibility)
    {
        if (visibility != horizontal_scroll_bar_visibility_)
        {
            horizontal_scroll_bar_visibility_ = visibility;
            switch (visibility)
            {
            case ScrollBarVisibility::Always:
                is_horizontal_scroll_bar_visible_ = true;
                break;
            case ScrollBarVisibility::None:
                is_horizontal_scroll_bar_visible_ = false;
                break;
            case ScrollBarVisibility::Auto:
                UpdateScrollBarVisibility();
            }
            InvalidateDraw();
        }
    }

    void ScrollControl::SetVerticalScrollBarVisibility(const ScrollBarVisibility visibility)
    {
        if (visibility != vertical_scroll_bar_visibility_)
        {
            vertical_scroll_bar_visibility_ = visibility;
            switch (visibility)
            {
            case ScrollBarVisibility::Always:
                is_vertical_scroll_bar_visible_ = true;
                break;
            case ScrollBarVisibility::None:
                is_vertical_scroll_bar_visible_ = false;
                break;
            case ScrollBarVisibility::Auto:
                UpdateScrollBarVisibility();
            }
            InvalidateDraw();
        }

    }

    void ScrollControl::SetScrollOffset(std::optional<float> x, std::optional<float> y)
    {
        CoerceAndSetOffsets(x.value_or(GetScrollOffsetX()), y.value_or(GetScrollOffsetY()));
    }

    void ScrollControl::SetViewWidth(const float length)
    {
        view_width_ = length;
    }

    void ScrollControl::SetViewHeight(const float length)
    {
        view_height_ = length;
    }

    Size ScrollControl::OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        const auto layout_params = GetLayoutParams();

        auto available_size_for_children = available_size;
        if (IsHorizontalScrollEnabled())
        {
            if (layout_params->width.mode == MeasureMode::Content)
                debug::DebugMessage(L"ScrollControl: Width measure mode is Content and horizontal scroll is enabled. So Stretch is used instead.");

            available_size_for_children.width = std::numeric_limits<float>::max();
        }

        if (IsVerticalScrollEnabled())
        {
            if (layout_params->height.mode == MeasureMode::Content)
                debug::DebugMessage(L"ScrollControl: Height measure mode is Content and vertical scroll is enabled. So Stretch is used instead.");

            available_size_for_children.height = std::numeric_limits<float>::max();
        }

        const auto child = GetChild();

        auto size = Size::Zero();
        if (child)
        {
            child->Measure(available_size_for_children, AdditionalMeasureInfo{false, false});
            size = child->GetDesiredSize();
        }


        auto result = size;
        if (IsHorizontalScrollEnabled())
        {
            SetViewWidth(size.width);
            result.width = available_size.width;
        }
        if (IsVerticalScrollEnabled())
        {
            SetViewHeight(size.height);
            result.height = available_size.height;
        }

        return result;
    }

    void ScrollControl::OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {
        auto layout_rect = rect;

        if (IsHorizontalScrollEnabled())
            layout_rect.width = GetViewWidth();
        if (IsVerticalScrollEnabled())
            layout_rect.height = GetViewHeight();

        const auto child = GetChild();

        if (child)
        {
            const auto layout_params = child->GetLayoutParams();
            const auto size = child->GetDesiredSize();

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

            child->Layout(Rect(Point(
                IsHorizontalScrollEnabled() ? layout_rect.left + offset_x_ : calculate_anchor(layout_rect.left, layout_params->width.alignment, layout_rect.width, size.width),
                IsVerticalScrollEnabled() ? layout_rect.top + offset_y_ : calculate_anchor(layout_rect.top, layout_params->height.alignment, layout_rect.height, size.height)
            ), size), additional_info);
        }
    }

    void ScrollControl::OnRectChange(const Rect& old_rect, const Rect& new_rect)
    {
        UpdateScrollBarBorderInfo();
        CoerceAndSetOffsets(offset_x_, offset_y_, false);
        UpdateScrollBarVisibility();
    }

    void ScrollControl::CoerceAndSetOffsets(const float offset_x, const float offset_y, const bool update_children)
    {
        const auto old_offset_x = offset_x_;
        const auto old_offset_y = offset_y_;

        const auto content_rect = GetRect(RectRange::Content);
        offset_x_ = Coerce(offset_x, 0.0f, AtLeast0(view_width_ - content_rect.width));
        offset_y_ = Coerce(offset_y, 0.0f, AtLeast0(view_height_ - content_rect.height));
        UpdateScrollBarBarInfo();

        if (update_children)
        {
            if (const auto child = GetChild())
            {
                const auto old_position = child->GetPositionRelative();
                child->SetRect(Rect(Point(
                    old_position.x + old_offset_x - offset_x_,
                    old_position.y + old_offset_y - offset_y_
                ), child->GetSize()));
            }
        }
        InvalidateDraw();
    }

    void ScrollControl::UpdateScrollBarVisibility()
    {
        const auto content_rect = GetRect(RectRange::Content);
        if (GetHorizontalScrollBarVisibility() == ScrollBarVisibility::Auto)
            is_horizontal_scroll_bar_visible_ = view_width_ > content_rect.width;
        if (GetVerticalScrollBarVisibility() == ScrollBarVisibility::Auto)
            is_vertical_scroll_bar_visible_ = view_height_ > content_rect.height;
    }

    void ScrollControl::UpdateScrollBarBorderInfo()
    {
        const auto content_rect = GetRect(RectRange::Content);
        horizontal_bar_info_.border = Rect(content_rect.left, content_rect.GetBottom() - scroll_bar_width, content_rect.width, scroll_bar_width);
        vertical_bar_info_.border = Rect(content_rect.GetRight() - scroll_bar_width , content_rect.top, scroll_bar_width, content_rect.height);
    }

    void ScrollControl::UpdateScrollBarBarInfo()
    {
        const auto content_rect = GetRect(RectRange::Content);
        {
            const auto& border = horizontal_bar_info_.border;
            if (view_width_ <= content_rect.width)
                horizontal_bar_info_.bar = border;
            else
            {
                const auto bar_length =  border.width * content_rect.width / view_width_;
                const auto offset = border.width * offset_x_ / view_width_;
                horizontal_bar_info_.bar = Rect(border.left + offset, border.top, bar_length, border.height);
            }
        }
        {
            const auto& border = vertical_bar_info_.border;
            if (view_height_ <= content_rect.height)
                vertical_bar_info_.bar = border;
            else
            {
                const auto bar_length =  border.height * content_rect.height / view_height_;
                const auto offset = border.height * offset_y_ / view_height_;
                vertical_bar_info_.bar = Rect(border.left, border.top + offset, border.width, bar_length);
            }
        }
    }
}
