#include "timer.hpp"

namespace cru::platform::native::win {
TimerManager::TimerManager(GodWindow* god_window) { god_window_ = god_window; }

UINT_PTR TimerManager::CreateTimer(const UINT milliseconds, const bool loop,
                                   const TimerAction& action) {
  const auto id = current_count_++;
  ::SetTimer(god_window_->GetHandle(), id, milliseconds, nullptr);
  map_.emplace(id, std::make_pair(loop, action));
  return id;
}

void TimerManager::KillTimer(const UINT_PTR id) {
  const auto find_result = map_.find(id);
  if (find_result != map_.cend()) {
    ::KillTimer(god_window_->GetHandle(), id);
    map_.erase(find_result);
  }
}

std::optional<std::pair<bool, TimerAction>> TimerManager::GetAction(
    const UINT_PTR id) {
  const auto find_result = map_.find(id);
  if (find_result == map_.cend()) return std::nullopt;
  return find_result->second;
}
}  // namespace cru::platform::native::win
