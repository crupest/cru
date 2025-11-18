#include "cru/base/platform/unix/EventLoop.h"
#include "cru/base/Guard.h"

#include <poll.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>

namespace cru::platform::unix {
int UnixTimerFile::GetReadFd() const { return this->read_fd_; }

UnixEventLoop::UnixEventLoop() : polls_(1), poll_actions_(1) {
  auto action_pipe = OpenUniDirectionalPipe(UnixPipeFlags::NonBlock);
  action_pipe_read_end_ = std::move(action_pipe.read);
  action_pipe_write_end_ = std::move(action_pipe.write);
  polls_[0].fd = action_pipe_read_end_;
  polls_[0].events = POLLIN;
  poll_actions_[0] = [](auto _) {};
}

void UnixEventLoop::QueueAction(std::function<void()> action) {
  if (std::this_thread::get_id() == running_thread_) {
    action();
  } else {
    auto pointer = new std::function<void()>(std::move(action));
    action_pipe_write_end_.Write(&pointer, sizeof(decltype(pointer)));
  }
}

int UnixEventLoop::Run() {
  running_thread_ = std::this_thread::get_id();

  while (!exit_code_) {
    int poll_timeout = -1;
    Guard after_each_round_event_guard(
        [this] { AfterEachRoundEvent_.Raise(nullptr); });

    if (CheckPoll()) {
      continue;
    }

    auto now = std::chrono::steady_clock::now();
    if (auto result = timer_registry_.Update(now)) {
      result->data();
      continue;
    }

    if (CheckActionPipe()) {
      continue;
    }

    if (auto next_timeout = timer_registry_.NextTimeout(now)) {
      poll_timeout = next_timeout->count();
    }

    auto result = ::poll(polls_.data(), polls_.size(), poll_timeout);
    if (result < 0) {
      throw Exception("Failed to poll in event loop.");
    }
  }

  return *exit_code_;
}

void UnixEventLoop::RequestQuit(int exit_code) {
  QueueAction([this, exit_code] { exit_code_ = exit_code; });
}

int UnixEventLoop::SetTimer(std::function<void()> action,
                            std::chrono::milliseconds timeout, bool repeat) {
  return timer_registry_.Add(std::move(action), timeout, repeat);
}

void UnixEventLoop::CancelTimer(int id) { timer_registry_.Remove(id); }

bool UnixEventLoop::CheckPoll() {
  auto iter = std::ranges::find_if(
      polls_, [](const pollfd& poll_fd) { return poll_fd.revents != 0; });

  if (iter == polls_.cend()) {
    return false;
  }

  auto& revents = iter->revents;
  if (revents != 0) {
    poll_actions_[iter - polls_.cbegin()](revents);
  }
  revents = 0;

  return true;
}

bool UnixEventLoop::CheckActionPipe() {
  std::function<void()>* pointer;
  constexpr size_t pointer_size = sizeof(decltype(pointer));
  auto rest = pointer_size;
  while (rest > 0) {
    auto result = action_pipe_read_end_.Read(&pointer, rest);

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

  (*pointer)();
  delete pointer;

  return true;
}

void UnixEventLoop::SetPoll(int fd, PollEvents events, PollHandler action) {
  for (auto& poll_fd : polls_) {
    if (poll_fd.fd == fd) {
      poll_fd.events = events;
      return;
    }
  }

  pollfd poll_fd;
  poll_fd.fd = fd;
  poll_fd.events = events;
  poll_fd.revents = 0;
  polls_.push_back(poll_fd);
  poll_actions_.push_back(std::move(action));
}

void UnixEventLoop::RemovePoll(int fd) {
  auto iter = std::ranges::find_if(
      polls_, [fd](const pollfd& poll_fd) { return poll_fd.fd == fd; });
  if (iter != polls_.cend()) {
    auto index = iter - polls_.cbegin();
    polls_.erase(iter);
    poll_actions_.erase(poll_actions_.cbegin() + index);
  }
}

}  // namespace cru::platform::unix
