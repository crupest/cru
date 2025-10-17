#pragma once
#include "cru/base/Event.h"

#include "cru/base/Base.h"
#include "cru/platform/gui/win/GodWindow.h"
#include "cru/platform/gui/win/WindowNativeMessageEventArgs.h"

#include <functional>
#include <unordered_map>

namespace cru::platform::gui::win {
enum class TimerType { Immediate, Timeout, Interval };

struct TimerInfo {
  TimerInfo(long long id, TimerType type, int period,
            std::function<void()> action, UINT_PTR native_timer_id = 0)
      : id(id),
        type(type),
        period(period),
        action(std::move(action)),
        native_timer_id(native_timer_id) {}

  long long id;
  TimerType type;
  int period;  // in milliseconds
  std::function<void()> action;
  UINT_PTR native_timer_id;
};

class TimerManager : public Object {
 public:
  TimerManager(GodWindow* god_window);

  CRU_DELETE_COPY(TimerManager)
  CRU_DELETE_MOVE(TimerManager)

  ~TimerManager() override = default;

  // Period is in milliseconds. When type is immediate, it is not checked and
  // used.
  long long SetTimer(TimerType type, int period, std::function<void()> action);
  void CancelTimer(long long id);

 private:
  void HandleGodWindowMessage(WindowNativeMessageEventArgs& args);

  void CreateNativeTimer(TimerInfo* info);
  void KillNativeTimer(TimerInfo* info);

 private:
  GodWindow* god_window_;

  EventRevokerListGuard event_guard_;

  long long next_id_ = 1;
  std::unordered_map<long long, TimerInfo> info_map_;
};
}  // namespace cru::platform::gui::win
