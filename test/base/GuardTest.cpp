#include "cru/base/Guard.h"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <utility>

static_assert(!std::is_copy_constructible_v<cru::Guard>);
static_assert(!std::is_copy_assignable_v<cru::Guard>);
static_assert(std::is_move_constructible_v<cru::Guard>);
static_assert(std::is_move_assignable_v<cru::Guard>);

TEST_CASE("Guard should run exit function on destruction.", "[guard]") {
  int calls = 0;

  {
    cru::Guard guard([&calls] { calls++; });
    REQUIRE(calls == 0);
  }

  REQUIRE(calls == 1);
}

TEST_CASE("Guard drop should cancel exit function.", "[guard]") {
  int calls = 0;

  {
    cru::Guard guard([&calls] { calls++; });
    guard.Drop();
  }

  REQUIRE(calls == 0);
}

TEST_CASE("Guard move construction should transfer exit function.", "[guard]") {
  int calls = 0;

  {
    cru::Guard original_guard([&calls] { calls++; });

    {
      cru::Guard moved_guard(std::move(original_guard));
      REQUIRE(calls == 0);
    }

    REQUIRE(calls == 1);
  }

  REQUIRE(calls == 1);
}

TEST_CASE("Guard move assignment should transfer exit function.", "[guard]") {
  int calls = 0;

  {
    cru::Guard moved_guard;

    {
      cru::Guard original_guard([&calls] { calls++; });
      moved_guard = std::move(original_guard);
    }

    REQUIRE(calls == 0);
  }

  REQUIRE(calls == 1);
}
