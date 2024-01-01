#include "cru/common/Event2.h"

#include <catch2/catch_test_macros.hpp>

using cru::Event2;

TEST_CASE("Event2", "[event2]") {
  Event2 event;

  int counter = 0;

  auto handler = [&counter] { counter++; };
  auto token = event.AddHandler(handler);

  auto handler2 = [&counter](decltype(event)::Context* context) { counter++; };

  SECTION("two handlers should work.") {
    event.Raise();
    REQUIRE(counter == 1);
    event.Raise();
    REQUIRE(counter == 2);

    event.AddHandler(handler2);
    event.Raise();
    REQUIRE(counter == 4);
  }

  SECTION("handler revoker should work.") {
    token.RevokeHandler();
    event.Raise();
    REQUIRE(counter == 0);

    token = event.AddHandler(handler);
    event.AddHandler(handler2);
    event.Raise();
    REQUIRE(counter == 2);

    token.RevokeHandler();
    event.Raise();
    REQUIRE(counter == 3);
  }
}
