#include "cru/common/HandlerRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

TEST_CASE("HandlerRegistry", "[handler_registry]") {
  using namespace cru;
  HandlerRegistry<void()> registry;

  int counter = 1;

  auto tag1 = registry.AddHandler([&counter] { counter++; });
  auto tag2 = registry.AddHandler([&counter] { counter++; });

  for (const auto& handler : registry) {
    handler();
  }

  REQUIRE(counter == 3);

  registry.RemoveHandler(tag1);

  for (const auto& handler : registry) {
    handler();
  }

  REQUIRE(counter == 4);

  registry.RemoveHandler(tag2);

  for (const auto& handler : registry) {
    handler();
  }

  REQUIRE(counter == 4);
}
