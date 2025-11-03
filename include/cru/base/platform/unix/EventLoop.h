#pragma once

#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../Exception.h"
#include "../../Timer.h"
#include "UnixFile.h"

#include <poll.h>
#include <unistd.h>
#include <chrono>
#include <optional>
#include <thread>
#include <utility>

namespace cru::platform::unix {
class UnixTimerFile : public Object {
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

class UnixEventLoop : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::unix::UnixEventLoop")
 public:
  using PollEvents = decltype(std::declval<pollfd>().events);
  using PollRevents = decltype(std::declval<pollfd>().revents);
  using PollHandler = std::function<void(PollEvents revent)>;

  UnixEventLoop();

  /**
   * Thread-safe.
   */
  void QueueAction(std::function<void()> action);

  int Run();
  void RequestQuit(int exit_code = 0);

  /**
   * Thread-safe.
   */
  int SetTimer(std::function<void()> action, std::chrono::milliseconds timeout,
               bool repeat);

  /**
   * Thread-safe.
   */
  void CancelTimer(int id);

  /**
   * Thread-safe.
   */
  int SetImmediate(std::function<void()> action) {
    return this->SetTimer(std::move(action), std::chrono::milliseconds::zero(),
                          false);
  }

  /**
   * Thread-safe.
   */
  int SetTimeout(std::function<void()> action,
                 std::chrono::milliseconds timeout) {
    return this->SetTimer(std::move(action), std::move(timeout), false);
  }

  /**
   * Thread-safe.
   */
  int SetInterval(std::function<void()> action,
                  std::chrono::milliseconds interval) {
    return this->SetTimer(std::move(action), std::move(interval), true);
  }

  void SetPoll(int fd, PollEvents events, PollHandler action);
  void RemovePoll(int fd);

 private:
  bool CheckPoll();
  bool CheckActionPipe();

 private:
  std::optional<std::thread::id> running_thread_;

  std::vector<pollfd> polls_;
  std::vector<PollHandler> poll_actions_;

  TimerRegistry<std::function<void()>> timer_registry_;

  UnixFileDescriptor action_pipe_read_end_;
  UnixFileDescriptor action_pipe_write_end_;

  std::optional<int> exit_code_;
};

}  // namespace cru::platform::unix
