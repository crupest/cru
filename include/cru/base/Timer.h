#pragma once

#include "Base.h"

#include <algorithm>
#include <chrono>
#include <list>
#include <mutex>
#include <optional>
#include <ranges>

namespace cru {
template <typename D>
class TimerRegistry : public Object {
 private:
  struct TimerData {
    int id;
    D data;
    std::chrono::steady_clock::time_point created;
    std::chrono::steady_clock::time_point last_check;
    std::chrono::milliseconds interval;
    bool repeat;

    TimerData(int id, std::chrono::milliseconds interval, bool repeat,
              std::chrono::steady_clock::time_point created, D data)
        : id(id),
          created(created),
          last_check(created),
          interval(interval),
          repeat(repeat),
          data(std::move(data)) {}

    std::chrono::milliseconds NextTimeout(
        std::chrono::steady_clock::time_point now) const {
      return interval == std::chrono::milliseconds::zero()
                 ? std::chrono::milliseconds::zero()
                 : std::chrono::duration_cast<std::chrono::milliseconds>(
                       interval - (now - created) % interval);
    }

    bool Update(std::chrono::steady_clock::time_point now) {
      auto next_trigger =
          interval == std::chrono::milliseconds::zero()
              ? last_check
              : last_check - (last_check - created) % interval + interval;
      if (now >= next_trigger) {
        last_check = next_trigger;
        return true;
      } else {
        last_check = now;
        return false;
      }
    }
  };

 public:
  struct UpdateResult {
    int id;
    D data;

    bool operator==(const UpdateResult&) const = default;
  };

 public:
  TimerRegistry() : next_id_(1) {}

  int Add(D data, std::chrono::milliseconds interval, bool repeat,
          std::chrono::steady_clock::time_point created =
              std::chrono::steady_clock::now()) {
    if (interval < std::chrono::milliseconds::zero()) {
      throw Exception("Timer interval can't be negative.");
    }
    if (repeat && interval == std::chrono::milliseconds::zero()) {
      throw Exception("Repeat timer interval can't be 0.");
    }

    std::unique_lock lock(mutex_);
    auto id = next_id_++;
    timers_.emplace_back(id, interval, repeat, created, std::move(data));
    return id;
  }

  void Remove(int id) {
    std::unique_lock lock(mutex_);
    timers_.remove_if([id](const TimerData& timer) { return timer.id == id; });
  }

  /**
   * Returns nullopt if there is no timer.
   */
  std::optional<std::chrono::milliseconds> NextTimeout(
      std::chrono::steady_clock::time_point now) {
    std::unique_lock lock(mutex_);

    if (timers_.empty()) return std::nullopt;

    return std::ranges::min(
        timers_ | std::views::transform([now](const TimerData& timer) {
          return timer.NextTimeout(now);
        }));
  }

  std::optional<UpdateResult> Update(
      std::chrono::steady_clock::time_point new_time) {
    std::unique_lock lock(mutex_);
    for (auto iter = timers_.begin(); iter != timers_.end(); ++iter) {
      auto& timer = *iter;
      if (timer.Update(new_time)) {
        if (timer.repeat) {
          return UpdateResult{timer.id, timer.data};
        } else {
          UpdateResult result{timer.id, std::move(timer.data)};
          timers_.erase(iter);  // We will return, so it's safe to erase here.
          return result;
        }
      }
    }
    return std::nullopt;
  }

 private:
  std::mutex mutex_;
  int next_id_;
  std::list<TimerData> timers_;
};
}  // namespace cru
