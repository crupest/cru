#include "timer.h"

namespace cru
{
    UINT_PTR TimerManager::CreateTimer(const UINT milliseconds, const bool loop, std::shared_ptr<Action<>> action)
    {
        const auto id = current_count_++;
        ::SetTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), 0, milliseconds, nullptr);
        if (loop)
            map_[id] = std::move(action);
        else
            map_[id] = std::make_shared<Action<>>([this, action, id]() mutable {
            (*action)();
            this->KillTimer(id);
        });
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

    std::shared_ptr<Action<>> TimerManager::GetAction(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result == map_.cend())
            return nullptr;
        return find_result->second;
    }

    class TimerTaskImpl : public ITimerTask
    {
    public:
        explicit TimerTaskImpl(UINT_PTR id);
        TimerTaskImpl(const TimerTaskImpl& other) = delete;
        TimerTaskImpl(TimerTaskImpl&& other) = delete;
        TimerTaskImpl& operator=(const TimerTaskImpl& other) = delete;
        TimerTaskImpl& operator=(TimerTaskImpl&& other) = delete;
        ~TimerTaskImpl() override = default;

        void Cancel() override;

    private:
        UINT_PTR id_;
    };

    TimerTaskImpl::TimerTaskImpl(const UINT_PTR id)
        : id_(id)
    {

    }

    void TimerTaskImpl::Cancel()
    {
        TimerManager::GetInstance()->KillTimer(id_);
    }

    inline UINT SecondToMillisecond(const double seconds)
    {
        return static_cast<UINT>(seconds * 1000);
    }

    std::shared_ptr<ITimerTask> SetTimeout(double seconds, std::shared_ptr<Action<>> action)
    {
        auto id = TimerManager::GetInstance()->CreateTimer(SecondToMillisecond(seconds), false, std::move(action));
        return std::make_shared<TimerTaskImpl>(id);
    }

    std::shared_ptr<ITimerTask> SetInterval(double seconds, std::shared_ptr<Action<>> action)
    {
        auto id = TimerManager::GetInstance()->CreateTimer(SecondToMillisecond(seconds), true, std::move(action));
        return std::make_shared<TimerTaskImpl>(id);
    }
}
