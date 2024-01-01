#include "cru/common/Event2.h"

#include <catch2/catch_test_macros.hpp>

using cru::Event2;

TEST_CASE("Event2 handlers should work.", "[event2]") {
  Event2 event;

  int counter = 0;

  auto handler = [&counter] { counter++; };

  event.AddHandler(handler);

  event.Raise();

  REQUIRE(counter == 1);
}
