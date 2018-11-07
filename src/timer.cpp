#include "timer.hpp"

#include "application.hpp"

namespace cru
{
    inline TimerManager* GetTimerManager()
    {
        return Application::GetInstance()->GetTimerManager();
    }

    UINT_PTR TimerManager::CreateTimer(const UINT milliseconds, const bool loop, const TimerAction& action)
    {
        const auto id = current_count_++;
        ::SetTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), id, milliseconds, nullptr);
        map_.emplace(id, std::make_pair(loop, action));
        return id;
    }

    void TimerManager::KillTimer(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result != map_.cend())
        {
            ::KillTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), id);
            map_.erase(find_result);
        }
    }

    std::optional<std::pair<bool, TimerAction>> TimerManager::GetAction(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result == map_.cend())
            return std::nullopt;
        return find_result->second;
    }

    TimerTask::TimerTask(const UINT_PTR id)
        : id_(id)
    {

    }

    void TimerTask::Cancel()
    {
        GetTimerManager()->KillTimer(id_);
    }

    TimerTask SetTimeout(std::chrono::milliseconds milliseconds, const TimerAction& action)
    {
        const auto id = GetTimerManager()->CreateTimer(static_cast<UINT>(milliseconds.count()), false, action);
        return TimerTask(id);
    }

    TimerTask SetInterval(std::chrono::milliseconds milliseconds, const TimerAction& action)
    {
        const auto id = GetTimerManager()->CreateTimer(static_cast<UINT>(milliseconds.count()), true, action);
        return TimerTask(id);
    }
}
