#pragma once
#include "pre.hpp"

#include <Windows.h>
#include <chrono>
#include <functional>
#include <map>
#include <optional>

#include "base.hpp"

namespace cru {
using TimerAction = std::function<void()>;

class TimerManager : public Object {
 public:
  static TimerManager* GetInstance();

 private:
  TimerManager() = default;

 public:
  TimerManager(const TimerManager& other) = delete;
  TimerManager(TimerManager&& other) = delete;
  TimerManager& operator=(const TimerManager& other) = delete;
  TimerManager& operator=(TimerManager&& other) = delete;
  ~TimerManager() override = default;

  UINT_PTR CreateTimer(UINT milliseconds, bool loop, const TimerAction& action);
  void KillTimer(UINT_PTR id);
  std::optional<std::pair<bool, TimerAction>> GetAction(UINT_PTR id);

 private:
  std::map<UINT_PTR, std::pair<bool, TimerAction>> map_{};
  UINT_PTR current_count_ = 0;
};

class TimerTask {
  friend TimerTask SetTimeout(std::chrono::milliseconds milliseconds,
                              const TimerAction& action);
  friend TimerTask SetInterval(std::chrono::milliseconds milliseconds,
                               const TimerAction& action);

 private:
  explicit TimerTask(UINT_PTR id);

 public:
  TimerTask(const TimerTask& other) = default;
  TimerTask(TimerTask&& other) = default;
  TimerTask& operator=(const TimerTask& other) = default;
  TimerTask& operator=(TimerTask&& other) = default;
  ~TimerTask() = default;

  void Cancel() const;

 private:
  UINT_PTR id_;
};

TimerTask SetTimeout(std::chrono::milliseconds milliseconds,
                     const TimerAction& action);
TimerTask SetInterval(std::chrono::milliseconds milliseconds,
                      const TimerAction& action);
}  // namespace cru
