#pragma once

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
            explicit MeasureLength(const float length = 0.0, const MeasureMode mode = MeasureMode::Content)
                : length(length), mode(mode)
            {

            }

            bool Validate() const
            {
                if (mode == MeasureMode::Exactly && length < 0.0)
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"MeasureLength validation error: mode is Exactly but length is less than 0.\n");
#endif
                    return false;
                }
                return true;
            }

            float length;
            MeasureMode mode;
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
                if (!width.Validate())
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"Width(MeasureLength) is not valid.");
#endif
                    return false;
                }
                if (!height.Validate())
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"Height(MeasureLength) is not valid.");
#endif
                    return false;
                }
                return true;
            }

            MeasureLength width;
            MeasureLength height;
        };
    }
}
