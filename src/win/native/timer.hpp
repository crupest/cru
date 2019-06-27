#pragma once
#include "cru/win/win_pre_config.hpp"

#include "cru/common/base.hpp"
#include "cru/win/native/god_window.hpp"

#include <chrono>
#include <functional>
#include <map>
#include <optional>

namespace cru::platform::native::win {
using TimerAction = std::function<void()>;

class TimerManager : public Object {
 public:
  TimerManager(GodWindow* god_window);
  TimerManager(const TimerManager& other) = delete;
  TimerManager(TimerManager&& other) = delete;
  TimerManager& operator=(const TimerManager& other) = delete;
  TimerManager& operator=(TimerManager&& other) = delete;
  ~TimerManager() override = default;

  UINT_PTR CreateTimer(UINT milliseconds, bool loop, const TimerAction& action);
  void KillTimer(UINT_PTR id);
  std::optional<std::pair<bool, TimerAction>> GetAction(UINT_PTR id);

 private:
  GodWindow* god_window_;

  std::map<UINT_PTR, std::pair<bool, TimerAction>> map_{};
  UINT_PTR current_count_ = 0;
};
}  // namespace cru::win::native
