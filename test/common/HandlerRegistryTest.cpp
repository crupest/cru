#include "cru/common/HandlerRegistry.h"

#include <gtest/gtest.h>
#include <algorithm>

TEST(HandlerRegistry, Work) {
  using namespace cru;
  HandlerRegistry<void()> registry;

  int counter = 1;

  auto tag1 = registry.AddHandler([&counter] { counter++; });
  auto tag2 = registry.AddHandler([&counter] { counter++; });

  for (const auto& handler : registry) {
    handler();
  }

  ASSERT_EQ(counter, 3);

  registry.RemoveHandler(tag1);

  for (const auto& handler : registry) {
    handler();
  }

  ASSERT_EQ(counter, 4);

  registry.RemoveHandler(tag2);

  for (const auto& handler : registry) {
    handler();
  }

  ASSERT_EQ(counter, 4);
}
