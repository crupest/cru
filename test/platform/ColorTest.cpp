#include "cru/platform/Color.h"

#include <catch2/catch_test_macros.hpp>

using cru::platform::Color;

TEST_CASE("Color Parse", "[color]") {
  REQUIRE(Color::Parse(u"blue") == Color::Parse(u"#0000ff"));
  REQUIRE(Color::Parse(u"#12abAB") == Color::FromHex(0x12abAB));
  REQUIRE(Color::Parse(u"#8812abAB") == Color::FromHexAlpha(0x8812abAB));
  REQUIRE(Color::Parse(u"averystrangestring") == std::nullopt);
  REQUIRE(Color::Parse(u"112233") == std::nullopt);
  REQUIRE(Color::Parse(u"#7777777") == std::nullopt);
}
