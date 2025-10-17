#include "cru/platform/Color.h"

#include <catch2/catch_test_macros.hpp>

using cru::platform::Color;

TEST_CASE("Color Parse", "[color]") {
  REQUIRE(Color::Parse("blue") == Color::Parse("#0000ff"));
  REQUIRE(Color::Parse("#12abAB") == Color::FromHex(0x12abAB));
  REQUIRE(Color::Parse("#8812abAB") == Color::FromHexAlpha(0x8812abAB));
  REQUIRE(Color::Parse("averystrangestring") == std::nullopt);
  REQUIRE(Color::Parse("112233") == std::nullopt);
  REQUIRE(Color::Parse("#7777777") == std::nullopt);
}
