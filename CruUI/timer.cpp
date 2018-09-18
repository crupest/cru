#include "timer.h"

namespace cru
{
    UINT_PTR TimerManager::CreateTimer(const UINT milliseconds, const bool loop, ActionPtr action)
    {
        const auto id = current_count_++;
        ::SetTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), id, milliseconds, nullptr);
        if (loop)
            map_[id] = std::move(action);
        else
            map_[id] = CreateActionPtr([this, action, id]() mutable {
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

    ActionPtr TimerManager::GetAction(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result == map_.cend())
            return nullptr;
        return find_result->second;
    }

    class TimerTaskImpl : public virtual ICancelable
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

    TimerTask SetTimeout(std::chrono::milliseconds milliseconds, ActionPtr action)
    {
        auto id = TimerManager::GetInstance()->CreateTimer(static_cast<UINT>(milliseconds.count()), false, std::move(action));
        return std::make_shared<TimerTaskImpl>(id);
    }

    TimerTask SetInterval(std::chrono::milliseconds milliseconds, ActionPtr action)
    {
        auto id = TimerManager::GetInstance()->CreateTimer(static_cast<UINT>(milliseconds.count()), true, std::move(action));
        return std::make_shared<TimerTaskImpl>(id);
    }
}
