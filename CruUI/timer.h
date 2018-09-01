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
    using TimerAction = std::function<void()>;

    class TimerManager : public Object
    {
        friend class cru::Application;
    private:
        static TimerManager* instance_;

    public:
        static TimerManager* GetInstance();

    public:
        TimerManager();
        TimerManager(const TimerManager& other) = delete;
        TimerManager(TimerManager&& other) = delete;
        TimerManager& operator=(const TimerManager& other) = delete;
        TimerManager& operator=(TimerManager&& other) = delete;
        ~TimerManager() override;

        UINT_PTR CreateTimer(UINT microseconds, bool loop, const TimerAction& action);
        void KillTimer(UINT_PTR id);
        std::optional<TimerAction> GetAction(UINT_PTR id);

    private:
        std::map<UINT_PTR, TimerAction> map_{};
    };

    struct ITimerTask : virtual Interface
    {
        virtual void Cancel() = 0;
    };

    std::shared_ptr<ITimerTask> SetTimeout(double seconds, const TimerAction& action);
    std::shared_ptr<ITimerTask> SetInterval(double seconds, const TimerAction& action);
}
