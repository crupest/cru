#include "cru/base/platform/unix/EventLoop.h"
#include "cru/base/Exception.h"

#include <poll.h>
#include <algorithm>
#include <chrono>
#include <thread>

namespace cru::platform::unix {
int UnixTimerFile::GetReadFd() const { return this->read_fd_; }

UnixEventLoop::UnixEventLoop() : polls_(1), poll_actions_(1), timer_tag_(1) {
  auto timer_pipe = OpenUniDirectionalPipe(UnixPipeFlags::NonBlock);
  timer_pipe_read_end_ = std::move(timer_pipe.read);
  timer_pipe_write_end_ = std::move(timer_pipe.write);
  polls_[0].fd = timer_pipe_read_end_;
  polls_[0].events = POLLIN;
  poll_actions_[0] = [](auto _) {};
}

int UnixEventLoop::Run() {
  running_thread_ = std::this_thread::get_id();

  while (!exit_code_) {
    int poll_timeout = -1;

    while (CheckPoll()) {
      continue;
    }

    while (CheckTimer()) {
      continue;
    }

    while (ReadTimerPipe()) {
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

    auto result = ::poll(polls_.data(), polls_.size(), poll_timeout);
    if (result < 0) {
      throw Exception("Failed to poll in event loop.");
    }

    auto end = std::chrono::steady_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    for (auto &timer : timers_) {
      timer.timeout -= time;
    }
  }

  return exit_code_.value();
}

void UnixEventLoop::RequestQuit(int exit_code) {
  exit_code_ = exit_code;
  if (std::this_thread::get_id() != running_thread_) {
    SetImmediate([] {});
  }
}

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
    auto timer =
        new TimerData(tag, std::move(timeout), repeat, std::move(action));
    timer_pipe_write_end_.Write(&timer, sizeof(decltype(timer)));
  }

  return tag;
}

void UnixEventLoop::CancelTimer(int id) {
  if (std::this_thread::get_id() == running_thread_) {
    RemoveTimer(id);
  } else {
    SetImmediate([this, id] { RemoveTimer(id); });
  }
}

bool UnixEventLoop::CheckPoll() {
  auto iter = std::ranges::find_if(
      polls_, [](const pollfd &poll_fd) { return poll_fd.revents != 0; });

  if (iter == polls_.cend()) {
    return false;
  }

  auto &revents = iter->revents;
  if (revents != 0) {
    poll_actions_[iter - polls_.cbegin()](revents);
  }
  revents = 0;

  return true;
}

bool UnixEventLoop::CheckTimer() {
  auto iter = std::ranges::find_if(timers_, [](const TimerData &timer) {
    return timer.timeout <= std::chrono::milliseconds::zero();
  });

  if (iter == timers_.end()) {
    return false;
  }

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

  return true;
}

bool UnixEventLoop::ReadTimerPipe() {
  TimerData *pointer;
  constexpr size_t pointer_size = sizeof(decltype(pointer));
  auto rest = pointer_size;
  while (rest > 0) {
    auto result = timer_pipe_read_end_.Read(&pointer, rest);

    if (result == -1) {  // If no data.
      if (rest == pointer_size) {
        return false;
      } else {
        continue;  // Try read again (might spin), as we are in the middle of
                   // reading a pointer.
      }
    }

    if (result == 0) {
      throw Exception("Unexpected EOF of the timer pipe.");
    }

    rest -= result;
  }

  timers_.push_back(std::move(*pointer));
  delete pointer;

  return true;
}

void UnixEventLoop::RemoveTimer(int id) {
  auto iter = std::ranges::find_if(
      timers_, [id](const TimerData &timer) { return timer.id == id; });
  if (iter != timers_.cend()) {
    timers_.erase(iter);
  }
}

}  // namespace cru::platform::unix
