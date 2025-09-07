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
  auto delay = std::chrono::steady_clock::now() - start;
  REQUIRE(delay > test_duration);
  REQUIRE(delay < std::chrono::milliseconds(500));
}

TEST_CASE("UnixEventLoop Timer", "[unix][time]") {
  using namespace cru;
  using namespace cru::platform::unix;

  UnixEventLoop loop;

  auto test_miliseconds = 300;
  auto test_duration = std::chrono::milliseconds(test_miliseconds);
  auto start = std::chrono::steady_clock::now();

  int counter = 0;

  loop.SetTimeout(
      [test_duration, start] {
        auto delay = std::chrono::steady_clock::now() - start;
        REQUIRE(delay > test_duration);
        REQUIRE(delay < std::chrono::milliseconds(500));
      },
      test_duration);

  int timer_id;
  timer_id = loop.SetInterval(
      [&loop, timer_id, test_duration, start, &counter] {
        switch (counter) {
          case 0: {
            auto delay = std::chrono::steady_clock::now() - start;
            REQUIRE(delay > test_duration);
            REQUIRE(delay < std::chrono::milliseconds(500));
            counter++;
            break;
          }
          case 1: {
            auto delay = std::chrono::steady_clock::now() - start;
            REQUIRE(delay > test_duration * 2);
            REQUIRE(delay < std::chrono::milliseconds(1000));
            counter++;
            break;
          }
          default: {
            loop.CancelTimer(timer_id);
            loop.RequestQuit();
            break;
          }
        }
      },
      test_duration);

  auto exit_code = loop.Run();
  REQUIRE(exit_code == 0);
  REQUIRE(counter == 2);
}
