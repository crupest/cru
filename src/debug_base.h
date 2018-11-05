#pragma once


#include "system_headers.h"
#include <functional>

#include "base.h"
#include "format.h"

namespace cru::debug
{
#ifdef CRU_DEBUG
    inline void DebugTime(const std::function<void()>& action, const StringView& hint_message)
    {
        const auto before = std::chrono::steady_clock::now();
        action();
        const auto after = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        OutputDebugStringW(Format(L"{}: {}ms.\n", hint_message, duration.count()).c_str());
    }

    template<typename TReturn>
    TReturn DebugTime(const std::function<TReturn()>& action, const StringView& hint_message)
    {
        const auto before = std::chrono::steady_clock::now();
        auto&& result = action();
        const auto after = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        OutputDebugStringW(Format(L"{}: {}ms.\n", hint_message, duration.count()).c_str());
        return std::move(result);
    }
#else
    inline void DebugTime(Function<void()>&& action, const StringView& hint_message)
    {
        action();
    }

    template<typename TReturn>
    TReturn DebugTime(Function<TReturn()>&& action, const StringView& hint_message)
    {
        return action();
    }
#endif
}
