#pragma once


#include "system_headers.h"
#include <memory>
#include <map>
#include <chrono>

#include "base.h"
#include "application.h"

namespace cru
{
    class TimerManager : public Object
    {
    public:
        static TimerManager* GetInstance()
        {
            return Application::GetInstance()->GetTimerManager();
        }

    public:
        TimerManager() = default;
        TimerManager(const TimerManager& other) = delete;
        TimerManager(TimerManager&& other) = delete;
        TimerManager& operator=(const TimerManager& other) = delete;
        TimerManager& operator=(TimerManager&& other) = delete;
        ~TimerManager() override = default;

        UINT_PTR CreateTimer(UINT milliseconds, bool loop, ActionPtr action);
        void KillTimer(UINT_PTR id);
        ActionPtr GetAction(UINT_PTR id);

    private:
        std::map<UINT_PTR, ActionPtr> map_{};
        UINT_PTR current_count_ = 0;
    };

    using TimerTask = CancelablePtr;

    TimerTask SetTimeout(std::chrono::milliseconds milliseconds, ActionPtr action);
    TimerTask SetInterval(std::chrono::milliseconds milliseconds, ActionPtr action);
}
