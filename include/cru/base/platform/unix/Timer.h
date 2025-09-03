#pragma once

#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../Exception.h"
#include "UnixFile.h"

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
}  // namespace cru::platform::unix
