#include "cru/base/platform/unix/EventLoop.h"
#include "cru/base/Exception.h"

#include <poll.h>
#include <algorithm>
#include <chrono>
#include <thread>

namespace cru::platform::unix {
int UnixTimerFile::GetReadFd() const { return this->read_fd_; }

UnixEventLoop::UnixEventLoop() : timer_tag_(1) {}

int UnixEventLoop::Run() {
  running_thread_ = std::this_thread::get_id();

  pollfd poll_fds[1];

  while (!exit_code_) {
    int poll_timeout = -1;

    auto iter = std::ranges::find_if(timers_, [](const TimerData &timer) {
      return timer.timeout <= std::chrono::milliseconds::zero();
    });
    if (iter != timers_.end()) {
      auto &timer = *iter;
      if (timer.repeat) {
        while (timer.timeout <= std::chrono::milliseconds::zero()) {
          timer.timeout += timer.original_timeout;
          timer.action();
        }
      } else {
        auto action = timer.action;
        timers_.erase(iter);
        action();
      }
      continue;
    }

    if (!timers_.empty()) {
      poll_timeout =
          std::ranges::min_element(timers_, [](const TimerData &left,
                                               const TimerData &right) {
            return left.timeout < right.timeout;
          })->timeout.count();
    }

    auto start = std::chrono::steady_clock::now();

    ::poll(poll_fds, sizeof poll_fds / sizeof *poll_fds, poll_timeout);

    // TODO: A Big Implement to handle X events.

    auto end = std::chrono::steady_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    for (auto &timer : timers_) {
      timer.timeout -= time;
    }
  }

  return exit_code_.value();
}

void UnixEventLoop::RequestQuit(int exit_code) {}

int UnixEventLoop::SetTimer(std::function<void()> action,
                            std::chrono::milliseconds timeout, bool repeat) {
  if (repeat) {
    if (timeout <= std::chrono::milliseconds::zero()) {
      throw Exception("Interval must be bigger than 0.");
    }
  } else {
    if (timeout < std::chrono::milliseconds::zero()) {
      throw Exception("Timeout must be at least 0.");
    }
  }

  auto tag = timer_tag_++;

  if (std::this_thread::get_id() == running_thread_) {
    timers_.push_back(
        TimerData(tag, std::move(timeout), repeat, std::move(action)));
  } else {
    // TODO: Implement
  }

  return tag;
}

}  // namespace cru::platform::unix
