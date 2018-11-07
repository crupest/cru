#pragma once

#include <unordered_set>

#include "base.h"
#include "ui_base.h"

namespace cru
{
    namespace ui
    {
        class Control;
        class Window;

        enum class Alignment
        {
            Center,
            Start,
            End
        };

        enum class MeasureMode
        {
            Exactly,
            Content,
            Stretch
        };

        enum class RectRange
        {
            Content, // content excluding padding, border and margin
            Padding, // only including content and padding
            HalfBorder, // including content, padding and half border
            FullBorder, // including content, padding and full border
            Margin // including content, padding, border and margin
        };

        struct Thickness
        {
            constexpr static Thickness Zero()
            {
                return Thickness(0);
            }

            constexpr Thickness() : Thickness(0) { }

            constexpr explicit Thickness(const float width)
                : left(width), top(width), right(width), bottom(width) { }

            constexpr explicit Thickness(const float horizontal, const float vertical)
                : left(horizontal), top(vertical), right(horizontal), bottom(vertical) { }

            constexpr Thickness(const float left, const float top, const float right, const float bottom)
                : left(left), top(top), right(right), bottom(bottom) { }

            float GetHorizontalTotal() const
            {
                return left + right;
            }

            float GetVerticalTotal() const
            {
                return top + bottom;
            }

            float Validate() const
            {
                return left >= 0.0 && top >= 0.0 && right >= 0.0 && bottom >= 0.0;
            }

            float left;
            float top;
            float right;
            float bottom;
        };

        struct LayoutSideParams final
        {
            constexpr static LayoutSideParams Exactly(const float length, const Alignment alignment = Alignment::Center)
            {
                return LayoutSideParams(MeasureMode::Exactly, length, alignment);
            }

            constexpr static LayoutSideParams Content(const Alignment alignment = Alignment::Center)
            {
                return LayoutSideParams(MeasureMode::Content, 0, alignment);
            }

            constexpr static LayoutSideParams Stretch(const Alignment alignment = Alignment::Center)
            {
                return LayoutSideParams(MeasureMode::Stretch, 0, alignment);
            }

            constexpr LayoutSideParams() = default;

            constexpr explicit LayoutSideParams(const MeasureMode mode, const float length, const Alignment alignment)
                : length(length), mode(mode), alignment(alignment)
            {

            }

            constexpr bool Validate() const
            {
                if (length < 0.0)
                    return false;
                if (min.has_value() && min.value() < 0.0)
                    return false;
                if (max.has_value() && max.value() < 0.0)
                    return false;
                if (min.has_value() && max.has_value() && min.value() > max.value())
                    return false;
                return true;
            }

            // only used in exactly mode, specify the exactly side length of content.
            float length = 0.0;
            MeasureMode mode = MeasureMode::Content;
            Alignment alignment = Alignment::Center;

            // min and max specify the min/max side length of content.
            // they are used as hint and respect the actual size that content needs.
            // when mode is exactly, length is coerced into the min-max range.
            std::optional<float> min = std::nullopt;
            std::optional<float> max = std::nullopt;
        };

        struct BasicLayoutParams final
        {
            BasicLayoutParams() = default;
            BasicLayoutParams(const BasicLayoutParams&) = default;
            BasicLayoutParams(BasicLayoutParams&&) = default;
            BasicLayoutParams& operator = (const BasicLayoutParams&) = default;
            BasicLayoutParams& operator = (BasicLayoutParams&&) = default;
            ~BasicLayoutParams() = default;

            bool Validate() const
            {
                return width.Validate() && height.Validate() && margin.Validate() && padding.Validate();
            }

            LayoutSideParams width;
            LayoutSideParams height;
            Thickness padding;
            Thickness margin;
        };


        class LayoutManager : public Object
        {
        public:
            static LayoutManager* GetInstance();

        public:
            LayoutManager() = default;
            LayoutManager(const LayoutManager& other) = delete;
            LayoutManager(LayoutManager&& other) = delete;
            LayoutManager& operator=(const LayoutManager& other) = delete;
            LayoutManager& operator=(LayoutManager&& other) = delete;
            ~LayoutManager() override = default;


            //*************** region: position cache ***************

            //Mark position cache of the control and its descendants invalid,
            //(which is saved as an auto-managed list internal)
            //and send a message to refresh them.
            void InvalidateControlPositionCache(Control* control);

            //Refresh position cache of the control and its descendants whose cache
            //has been marked as invalid.
            void RefreshInvalidControlPositionCache();

            //Refresh position cache of the control and its descendants immediately.
            static void RefreshControlPositionCache(Control* control);


            //*************** region: layout ***************

            void InvalidateWindowLayout(Window* window);

            void RefreshInvalidWindowLayout();

        private:
            static void RefreshControlPositionCacheInternal(Control* control, const Point& parent_lefttop_absolute);

        private:
            std::unordered_set<Control*> cache_invalid_controls_;
            std::unordered_set<Window*> layout_invalid_windows_;
        };
    }
}
