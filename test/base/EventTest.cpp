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

TEST_CASE("Event destroy during raising.", "[event]") {
  using cru::Event;

  Event<int&>* event = new Event<int&>();

  event->AddSpyOnlyHandler([event] { delete event; });
  auto revoker = event->AddHandler([](int& count) { count++; });

  int count = 0;
  event->Raise(count);
  REQUIRE(count == 1);
  revoker();
}
