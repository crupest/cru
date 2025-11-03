#include "cru/base/Timer.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>

using cru::TimerRegistry;
using std::chrono::milliseconds;
using namespace std::chrono_literals;

TEST_CASE("TimerRegistry Works", "[timer]") {
  TimerRegistry<int> registry;

  REQUIRE_THROWS(registry.Add(1, -1ms, false));
  REQUIRE_THROWS(registry.Add(1, 0ms, true));

  auto mock_now = std::chrono::steady_clock::now();

  auto timer1 = registry.Add(1, 50ms, false, mock_now);
  auto timer2 = registry.Add(2, 100ms, false, mock_now);
  auto timer3 = registry.Add(3, 60ms, true, mock_now);
  auto timer4 = registry.Add(4, 100ms, true, mock_now);

  mock_now += 20ms;
  REQUIRE(registry.Update(mock_now) == std::nullopt);
  REQUIRE(registry.NextTimeout(mock_now) == 30ms);

  mock_now += 30ms;
  REQUIRE(registry.Update(mock_now) ==
          TimerRegistry<int>::UpdateResult{timer1, 1});
  REQUIRE(registry.Update(mock_now) == std::nullopt);
  REQUIRE(registry.NextTimeout(mock_now) == 10ms);

  mock_now += 20ms;
  REQUIRE(registry.Update(mock_now) ==
          TimerRegistry<int>::UpdateResult{timer3, 3});
  REQUIRE(registry.Update(mock_now) == std::nullopt);
  REQUIRE(registry.NextTimeout(mock_now) == 30ms);

  mock_now += 200ms;
  int count = 0;
  while (registry.Update(mock_now)) { count++;}
  REQUIRE(count == 6);
}
