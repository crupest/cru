#pragma once


#include "system_headers.h"
#include <functional>
#include <memory>
#include <map>
#include <optional>

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

        UINT_PTR CreateTimer(UINT milliseconds, bool loop, std::shared_ptr<Action<>> action);
        void KillTimer(UINT_PTR id);
        std::shared_ptr<Action<>> GetAction(UINT_PTR id);

    private:
        std::map<UINT_PTR, std::shared_ptr<Action<>>> map_{};
        UINT_PTR current_count_ = 0;
    };

    struct ITimerTask : virtual Interface
    {
        virtual void Cancel() = 0;
    };

    std::shared_ptr<ITimerTask> SetTimeout(double seconds, std::shared_ptr<Action<>> action);
    std::shared_ptr<ITimerTask> SetInterval(double seconds, std::shared_ptr<Action<>> action);
}
