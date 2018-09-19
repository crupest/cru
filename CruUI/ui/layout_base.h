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


        enum class MeasureMode
        {
            Exactly,
            Content,
            Stretch
        };

        struct MeasureLength final
        {
            constexpr static MeasureLength Exactly(const float length)
            {
                return MeasureLength(MeasureMode::Exactly, length);
            }

            constexpr static MeasureLength Content()
            {
                return MeasureLength(MeasureMode::Content, 0);
            }

            constexpr static MeasureLength Stretch()
            {
                return MeasureLength(MeasureMode::Stretch, 0);
            }

            constexpr MeasureLength() = default;

            constexpr explicit MeasureLength(const MeasureMode mode, const float length)
                : length(length), mode(mode)
            {

            }

            constexpr bool Validate() const
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

            float length = 0.0;
            MeasureMode mode = MeasureMode::Content;
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
