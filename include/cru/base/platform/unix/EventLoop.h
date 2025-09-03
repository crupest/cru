#pragma once

#include <optional>
#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../Exception.h"
#include "UnixFile.h"

#include <unistd.h>
#include <chrono>
#include <thread>

namespace cru::platform::unix {
class UnixTimerFile : public Object2 {
 public:
  template <class Rep, class Period>
  explicit UnixTimerFile(std::chrono::duration<Rep, Period> time) {
    auto fds = OpenUniDirectionalPipe();
    this->read_fd_ = std::move(fds.read);
    this->write_fd_ = std::move(fds.write);

    this->thread_ = std::thread([this, time] {
      std::this_thread::sleep_for(time);
      constexpr auto buffer = "";
      auto written = ::write(this->write_fd_, buffer, 1);
      if (written != 1) {
        throw Exception(
            "Failed to write to pipe in UnixTimerFile thread at timeout.");
      }
    });
    this->thread_.detach();
  }

  int GetReadFd() const;

 private:
  UnixFileDescriptor write_fd_;
  UnixFileDescriptor read_fd_;
  std::thread thread_;
};

class UnixEventLoop : public Object2 {
 public:
  UnixEventLoop();

  int Run();
  void RequestQuit(int exit_code = 0);

  int SetTimer(std::function<void()> action, std::chrono::milliseconds timeout,
               bool repeat);

  int SetImmediate(std::function<void()> action) {
    return this->SetTimer(std::move(action), std::chrono::milliseconds::zero(),
                          false);
  }

  int SetTimeout(std::function<void()> action,
                 std::chrono::milliseconds timeout) {
    return this->SetTimer(std::move(action), std::move(timeout), false);
  }

  int SetInterval(std::function<void()> action,
                  std::chrono::milliseconds interval) {
    return this->SetTimer(std::move(action), std::move(interval), true);
  }

 private:
  struct TimerData {
    int id;
    std::chrono::milliseconds original_timeout;
    std::chrono::milliseconds timeout;
    bool repeat;
    std::function<void()> action;

    TimerData(int id, std::chrono::milliseconds timeout, bool repeat,
              std::function<void()> action)
        : id(id),
          original_timeout(timeout),
          timeout(timeout),
          repeat(repeat),
          action(std::move(action)) {}
  };

  std::thread::id running_thread_;

  std::atomic_int timer_tag_;
  std::vector<TimerData> timers_;

  std::optional<int> exit_code_;
};

}  // namespace cru::platform::unix
