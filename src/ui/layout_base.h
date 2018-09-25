#pragma once

#include "system_headers.h"
#include <unordered_set>

#include "base.h"
#include "ui_base.h"

namespace cru
{
    namespace ui
    {
        class Control;

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
                if (mode == MeasureMode::Exactly && length < 0.0)
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"LayoutSideParams validation error: mode is Exactly but length is less than 0.\n");
#endif
                    return false;
                }
                return true;
            }

            float length = 0.0;
            MeasureMode mode = MeasureMode::Content;
            Alignment alignment = Alignment::Center;
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
                if (!width.Validate())
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"Width(LayoutSideParams) is not valid.");
#endif
                    return false;
                }
                if (!height.Validate())
                {
#ifdef CRU_DEBUG
                    ::OutputDebugStringW(L"Height(LayoutSideParams) is not valid.");
#endif
                    return false;
                }
                return true;
            }

            LayoutSideParams width;
            LayoutSideParams height;
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

            //Mark position cache of the control and its descendants invalid,
            //(which is saved as an auto-managed list internal)
            //and send a message to refresh them.
            void InvalidateControlPositionCache(Control* control);

            //Refresh position cache of the control and its descendants whose cache
            //has been marked as invalid.
            void RefreshInvalidControlPositionCache();

            //Refresh position cache of the control and its descendants immediately.
            static void RefreshControlPositionCache(Control* control);

        private:
            static void RefreshControlPositionCacheInternal(Control* control, const Point& parent_lefttop_absolute);

        private:
            std::unordered_set<Control*> cache_invalid_controls_;
        };
    }
}
