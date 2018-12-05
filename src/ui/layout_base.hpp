#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "ui_base.hpp"

namespace cru::ui
{
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
}
