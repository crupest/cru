#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include <optional>
#include <initializer_list>

#include "ui/control.hpp"

namespace cru::ui::controls
{
    // Done: OnMeasureContent
    // Done: OnLayoutContent
    // Done: HitTest(no need)
    // Done: Draw(no need)
    // Done: API
    // Done: ScrollBar
    // Done: MouseEvent
    class ScrollControl : public ContentControl
    {
    private:
        struct ScrollBarInfo
        {
            Rect border = Rect();
            Rect bar = Rect();
        };

        enum class Orientation
        {
            Horizontal,
            Vertical
        };

    public:
        enum class ScrollBarVisibility
        {
            None,
            Auto,
            Always
        };

        static ScrollControl* Create(Control* child = nullptr)
        {
            const auto control = new ScrollControl(true);
            control->SetChild(child);
            return control;
        }

        static constexpr auto control_type = L"ScrollControl";

    protected:
        explicit ScrollControl(bool container);
    public:
        ScrollControl(const ScrollControl& other) = delete;
        ScrollControl(ScrollControl&& other) = delete;
        ScrollControl& operator=(const ScrollControl& other) = delete;
        ScrollControl& operator=(ScrollControl&& other) = delete;
        ~ScrollControl() override;

        StringView GetControlType() const override final;

        bool IsHorizontalScrollEnabled() const
        {
            return horizontal_scroll_enabled_;
        }

        void SetHorizontalScrollEnabled(bool enable);

        bool IsVerticalScrollEnabled() const
        {
            return vertical_scroll_enabled_;
        }

        void SetVerticalScrollEnabled(bool enable);


        ScrollBarVisibility GetHorizontalScrollBarVisibility() const
        {
            return horizontal_scroll_bar_visibility_;
        }

        void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

        ScrollBarVisibility GetVerticalScrollBarVisibility() const
        {
            return vertical_scroll_bar_visibility_;
        }

        void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

        float GetViewWidth() const
        {
            return view_width_;
        }

        float GetViewHeight() const
        {
            return view_height_;
        }

        float GetScrollOffsetX() const
        {
            return offset_x_;
        }

        float GetScrollOffsetY() const
        {
            return offset_y_;
        }

        // nullopt for not set. value is auto-coerced.
        void SetScrollOffset(std::optional<float> x, std::optional<float> y);

    protected:
        void SetViewWidth(float length);
        void SetViewHeight(float length);

        Size OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info) override final;
        void OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info) override final;

        void OnRectChange(const Rect& old_rect, const Rect& new_rect) override;

    private:
        void CoerceAndSetOffsets(float offset_x, float offset_y, bool update_children = true);
        void UpdateScrollBarVisibility();
        void UpdateScrollBarBorderInfo();
        void UpdateScrollBarBarInfo();

    private:
        bool horizontal_scroll_enabled_ = true;
        bool vertical_scroll_enabled_ = true;

        ScrollBarVisibility horizontal_scroll_bar_visibility_ = ScrollBarVisibility::Auto;
        ScrollBarVisibility vertical_scroll_bar_visibility_ = ScrollBarVisibility::Auto;

        bool is_horizontal_scroll_bar_visible_ = false;
        bool is_vertical_scroll_bar_visible_ = false;

        float offset_x_ = 0.0f;
        float offset_y_ = 0.0f;

        float view_width_ = 0.0f;
        float view_height_ = 0.0f;

        ScrollBarInfo horizontal_bar_info_;
        ScrollBarInfo vertical_bar_info_;

        std::optional<Orientation> is_pressing_scroll_bar_ = std::nullopt;
        float pressing_delta_ = 0.0f;
    };
}
