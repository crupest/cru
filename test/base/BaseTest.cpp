#include "cru/base/Base.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using cru::Exception;
using cru::Index;

namespace {
template <typename Action>
void RequireThrowsWithArgumentName(Action action,
                                   std::string_view argument_name) {
  try {
    action();
    FAIL("Expected CheckArgument to throw");
  } catch (const Exception& exception) {
    REQUIRE(std::string_view(exception.what()).find(argument_name) !=
            std::string_view::npos);
  }
}
}  // namespace

TEST_CASE("CheckArgumentNonNull validates null arguments", "[base]") {
  int value = 0;
  int* argument = &value;

  REQUIRE_NOTHROW([&] { CheckArgumentNonNull(argument); }());

  argument = nullptr;
  RequireThrowsWithArgumentName([&] { CheckArgumentNonNull(argument); },
                                "argument");
}

TEST_CASE("CheckArgumentNoLessThan validates minimum value", "[base]") {
  Index argument = 5;
  Index min = 5;

  REQUIRE_NOTHROW([&] { CheckArgumentNoLessThan(argument, min); }());

  argument = 4;
  RequireThrowsWithArgumentName([&] { CheckArgumentNoLessThan(argument, min); },
                                "argument");
}

TEST_CASE("CheckArgumentNoGreaterThan validates maximum value", "[base]") {
  Index argument = 5;
  Index max = 5;

  REQUIRE_NOTHROW([&] { CheckArgumentNoGreaterThan(argument, max); }());

  argument = 6;
  RequireThrowsWithArgumentName(
      [&] { CheckArgumentNoGreaterThan(argument, max); }, "argument");
}

TEST_CASE("CheckArgumentRange validates inclusive range", "[base]") {
  Index argument = 2;
  Index min = 2;
  Index max = 5;

  REQUIRE_NOTHROW([&] { CheckArgumentRange(argument, min, max); }());

  argument = 5;
  REQUIRE_NOTHROW([&] { CheckArgumentRange(argument, min, max); }());

  argument = 1;
  RequireThrowsWithArgumentName([&] { CheckArgumentRange(argument, min, max); },
                                "argument");

  argument = 6;
  RequireThrowsWithArgumentName([&] { CheckArgumentRange(argument, min, max); },
                                "argument");
}
