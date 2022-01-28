#include "TimerManager.hpp"

#include "cru/win/gui/Base.hpp"
#include "cru/win/gui/Exception.hpp"
#include "gsl/gsl_util"

#include <functional>
#include <type_traits>

namespace cru::platform::gui::win {
constexpr int kSetImmediateWindowMessageId = WM_USER + 2000;

TimerManager::TimerManager(GodWindow* god_window) {
  god_window_ = god_window;
  event_guard_ += god_window->MessageEvent()->AddHandler(std::bind(
      &TimerManager::HandleGodWindowMessage, this, std::placeholders::_1));
}

long long TimerManager::SetTimer(TimerType type, int period,
                                 std::function<void()> action) {
  auto id = next_id_++;
  TimerInfo timer_info{id, type, type == TimerType::Immediate ? 0 : period,
                       std::move(action)};
  if (type == TimerType::Immediate) {
    if (!::PostMessageW(god_window_->GetHandle(), kSetImmediateWindowMessageId,
                        gsl::narrow<UINT_PTR>(id), 0)) {
      throw Win32Error(
          ::GetLastError(),
          u"Failed to post window message to god window for set immediate.");
    }
  } else {
    CreateNativeTimer(&timer_info);
  }

  info_map_.emplace(id, std::move(timer_info));
  return id;
}

void TimerManager::CancelTimer(long long id) {
  if (id <= 0) return;
  auto find_result = this->info_map_.find(id);
  if (find_result != info_map_.cend()) {
    auto& info = find_result->second;
    KillNativeTimer(&info);
    this->info_map_.erase(find_result);
  }
}

void TimerManager::CreateNativeTimer(TimerInfo* info) {
  info->native_timer_id = gsl::narrow<UINT_PTR>(info->id);
  ::SetTimer(god_window_->GetHandle(), info->native_timer_id, info->period,
             nullptr);
}

void TimerManager::KillNativeTimer(TimerInfo* info) {
  if (info->native_timer_id == 0) return;
  ::KillTimer(god_window_->GetHandle(), info->native_timer_id);
  info->native_timer_id = 0;
}

void TimerManager::HandleGodWindowMessage(WindowNativeMessageEventArgs& args) {
  const auto& message = args.GetWindowMessage();

  switch (message.msg) {
    case kSetImmediateWindowMessageId: {
      auto find_result =
          this->info_map_.find(static_cast<long long>(message.w_param));
      if (find_result != info_map_.cend()) {
        auto& info = find_result->second;
        info.action();
        info_map_.erase(find_result);
      }
      args.SetResult(0);
      args.SetHandled(true);
      return;
    }
    case WM_TIMER: {
      auto find_result =
          this->info_map_.find(static_cast<long long>(message.w_param));
      if (find_result != info_map_.cend()) {
        auto& info = find_result->second;
        if (info.type == TimerType::Interval) {
          info.action();
          args.SetResult(0);
          args.SetHandled(true);
        } else if (info.type == TimerType::Timeout) {
          info.action();
          KillNativeTimer(&info);
          info_map_.erase(find_result);
          args.SetResult(0);
          args.SetHandled(true);
        }
      }
      return;
    }
    default:
      return;
  }
}
}  // namespace cru::platform::gui::win
