#pragma once

#include "ui/control.hpp"

namespace cru::ui::controls
{
    // Done: OnMeasureContent
    // Done: OnLayoutContent
    // TODO: HitTest
    // TODO: Draw
    // TODO: ScrollBar
    // TODO: MouseEvent
    class ScrollControl : public Control
    {
    public:
        enum class ScrollBarVisibility
        {
            None,
            Auto,
            Always
        };

    protected:
        explicit ScrollControl(bool container);
    public:
        ScrollControl(const ScrollControl& other) = delete;
        ScrollControl(ScrollControl&& other) = delete;
        ScrollControl& operator=(const ScrollControl& other) = delete;
        ScrollControl& operator=(ScrollControl&& other) = delete;
        ~ScrollControl() override;


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


        ScrollBarVisibility GetHorizontalScrollBarVisibility() const;
        void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);
        ScrollBarVisibility GetVerticalScrollBarVisibility() const;
        void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

        Control* HitTest(const Point& point) override final;

    protected:
        float GetViewWidth() const
        {
            return view_width_;
        }

        float GetViewHeight() const
        {
            return view_height_;
        }

        void SetViewWidth(float length);
        void SetViewHeight(float length);

        Size OnMeasureContent(const Size& available_size) override final;
        void OnLayoutContent(const Rect& rect) override final;

    private:
        bool horizontal_scroll_enabled_ = true;
        bool vertical_scroll_enabled_ = true;

        ScrollBarVisibility horizontal_scroll_bar_visibility_ = ScrollBarVisibility::Auto;
        ScrollBarVisibility vertical_scroll_bar_visibility_ = ScrollBarVisibility::Auto;

        float offset_x_ = 0.0f;
        float offset_y_ = 0.0f;

        float view_width_ = 0.0f;
        float view_height_ = 0.0f;
    };
}
