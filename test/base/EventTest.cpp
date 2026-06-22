#include "cru/base/Event.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Event should work.", "[event]") {
  using cru::Event;

  Event<int&> event;

  int count = 0;
  bool triggered = false;

  auto revoker = event.AddHandler([](int& count) { count++; });
  event.AddHandler([](int& count) { count++; });
  event.AddSpyOnlyHandler([&triggered] { triggered = true; });

  event.Raise(count);
  REQUIRE(count == 2);
  REQUIRE(triggered);

  revoker();

  event.Raise(count);
  REQUIRE(count == 3);
}

TEST_CASE("Event handler state should be preserved.", "[event]") {
  using cru::Event;

  int counter1 = 0;
  Event<int&>* event = new Event<int&>();
  event->AddHandler([counter2 = 0](int& counter1) mutable {
    counter1++;
    counter2++;
    REQUIRE(counter1 == counter2);
  });

  event->Raise(counter1);
  REQUIRE(counter1 == 1);
  event->Raise(counter1);
  REQUIRE(counter1 == 2);
}
