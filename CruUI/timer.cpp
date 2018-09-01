#include "timer.h"

namespace cru
{
	TimerManager* TimerManager::instance_ = nullptr;

	TimerManager* TimerManager::GetInstance()
	{
		return instance_;
	}

	TimerManager::TimerManager()
	{
		instance_ = this;
	}

	TimerManager::~TimerManager()
	{
		instance_ = nullptr;
	}

	UINT_PTR TimerManager::CreateTimer(const UINT microseconds, const bool loop, const TimerAction & action)
	{
		auto id = ::SetTimer(nullptr, 0, microseconds, nullptr);
		if (loop)
			map_[id] = action;
		else
			map_[id] = [this, action, id]() {
				action();
				this->KillTimer(id);
			};

		return id;
	}

	void TimerManager::KillTimer(const UINT_PTR id)
	{
		const auto find_result = map_.find(id);
		if (find_result != map_.cend())
		{
			::KillTimer(nullptr, id);
			map_.erase(find_result);
		}
	}

	std::optional<TimerAction> TimerManager::GetAction(const UINT_PTR id)
	{
		auto find_result = map_.find(id);
		if (find_result == map_.cend())
			return std::nullopt;
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

	inline UINT SecondToMicroSecond(const double seconds)
	{
		return static_cast<UINT>(seconds * 1000);
	}

	std::shared_ptr<ITimerTask> SetTimeout(const double seconds, const TimerAction & action)
	{
		auto id = TimerManager::GetInstance()->CreateTimer(SecondToMicroSecond(seconds), false, action);
		return std::make_shared<TimerTaskImpl>(id);
	}

	std::shared_ptr<ITimerTask> SetInterval(const double seconds, const TimerAction & action)
	{
		auto id = TimerManager::GetInstance()->CreateTimer(SecondToMicroSecond(seconds), true, action);
		return std::make_shared<TimerTaskImpl>(id);
	}
}
