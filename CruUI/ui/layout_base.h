#pragma once

#include <optional>

namespace cru
{
    namespace ui
    {
        enum class MeasureMode
        {
            Exactly,
            Content,
            Stretch
        };

        struct MeasureLength final
        {
            explicit MeasureLength(const float length = 0.0, const MeasureMode mode = MeasureMode::Exactly)
                : length(length), mode(mode)
            {

            }

            bool Validate() const
            {
                return !(mode == MeasureMode::Exactly && length < 0.0);
            }

            float length;
            MeasureMode mode;
        };

        struct MeasureSize final
        {
            MeasureLength width;
            MeasureLength height;

            bool Validate() const
            {
                return width.Validate() && height.Validate();
            }
        };

        struct OptionalSize final
        {
            OptionalSize()
                : width(std::nullopt), height(std::nullopt)
            {

            }

            OptionalSize(const std::optional<float> width, const std::optional<float> height)
                : width(width), height(height)
            {

            }

            OptionalSize(const OptionalSize& other) = default;
            OptionalSize(OptionalSize&& other) = default;
            OptionalSize& operator = (const OptionalSize& other) = default;
            OptionalSize& operator = (OptionalSize&& other) = default;
            ~OptionalSize() = default;

            bool Validate() const
            {
                if (width.has_value() && width.value() < 0.0)
                    return false;
                if (height.has_value() && height.value() < 0.0)
                    return false;
                return true;
            }

            std::optional<float> width;
            std::optional<float> height;
        };

        struct BasicLayoutParams
        {
            BasicLayoutParams() = default;
            BasicLayoutParams(const BasicLayoutParams&) = default;
            BasicLayoutParams(BasicLayoutParams&&) = default;
            BasicLayoutParams& operator = (const BasicLayoutParams&) = default;
            BasicLayoutParams& operator = (BasicLayoutParams&&) = default;
            virtual ~BasicLayoutParams() = default;

            bool Validate() const
            {
                return size.Validate() && max_size.Validate() && min_size.Validate();
            }

            MeasureSize size;
            OptionalSize min_size;
            OptionalSize max_size;
        };
    }
}