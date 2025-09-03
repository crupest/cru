#include "cru/base/platform/unix/EventLoop.h"

#include <catch2/catch_test_macros.hpp>

#include <poll.h>
#include <chrono>

TEST_CASE("UnixTimerFile Work", "[unix][time]") {
  using namespace cru;
  using namespace cru::platform::unix;

  auto test_miliseconds = 300;
  auto test_duration = std::chrono::milliseconds(test_miliseconds);
  auto start = std::chrono::steady_clock::now();
  REQUIRE((std::chrono::steady_clock::now() - start) < test_duration);

  UnixTimerFile timer(test_duration);

  struct pollfd fds[1];
  fds[0].fd = timer.GetReadFd();
  fds[0].events = POLLIN;
  REQUIRE(::poll(fds, 1, test_miliseconds * 2) == 1);
  REQUIRE((std::chrono::steady_clock::now() - start) > test_duration);
}
