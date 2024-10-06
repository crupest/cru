#include "cru/base/Exception.h"
#include "cru/base/StringToNumberConverter.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("StringToIntegerConverterImpl Base0", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter({}, 0);
  Index processed_characters_count;

  REQUIRE(converter.Parse("12345678", &processed_characters_count) ==
          StringToIntegerResult(false, 12345678));
  REQUIRE(processed_characters_count == 8);

  REQUIRE(converter.Parse("0", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 1);

  REQUIRE(converter.Parse("012", &processed_characters_count) ==
          StringToIntegerResult(false, 012));
  REQUIRE(processed_characters_count == 3);

  REQUIRE(converter.Parse("0x12", &processed_characters_count) ==
          StringToIntegerResult(false, 0x12));
  REQUIRE(processed_characters_count == 4);

  REQUIRE(converter.Parse("0X12", &processed_characters_count) ==
          StringToIntegerResult(false, 0x12));
  REQUIRE(processed_characters_count == 4);

  REQUIRE(converter.Parse("0b101", &processed_characters_count) ==
          StringToIntegerResult(false, 0b101));
  REQUIRE(processed_characters_count == 5);

  REQUIRE(converter.Parse("0B101", &processed_characters_count) ==
          StringToIntegerResult(false, 0b101));
  REQUIRE(processed_characters_count == 5);

  REQUIRE(converter.Parse("-123", &processed_characters_count) ==
          StringToIntegerResult(true, 123));
  REQUIRE(processed_characters_count == 4);

  REQUIRE(converter.Parse("-0x10", &processed_characters_count) ==
          StringToIntegerResult(true, 0x10));
  REQUIRE(processed_characters_count == 5);
}

TEST_CASE("StringToIntegerConverterImpl Base0ForError", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter({}, 0);
  Index processed_characters_count;

  REQUIRE(converter.Parse("a", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);

  REQUIRE(converter.Parse("0a", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);

  REQUIRE(converter.Parse("0x", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);

  REQUIRE(converter.Parse("0xx", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);

  REQUIRE(converter.Parse(" 0", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);

  REQUIRE(converter.Parse("0 ", &processed_characters_count) ==
          StringToIntegerResult(false, 0));
  REQUIRE(processed_characters_count == 0);
}

TEST_CASE("StringToIntegerConverterImpl ThrowOnErrorFlag", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(StringToNumberFlags::kThrowOnError, 0);
  Index processed_characters_count;
  REQUIRE_THROWS_AS(converter.Parse("?", &processed_characters_count),
                    Exception);
}

TEST_CASE("StringToIntegerConverterImpl AllowLeadingZeroFlag", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(
      StringToNumberFlags::kAllowLeadingSpaces, 0);
  Index processed_characters_count;
  REQUIRE(converter.Parse("   123", &processed_characters_count) ==
          StringToIntegerResult(false, 123));
  REQUIRE(processed_characters_count == 6);
}

TEST_CASE("StringToIntegerConverterImpl AllowTrailingZeroFlag", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(
      StringToNumberFlags::kAllowTrailingSpaces, 0);
  Index processed_characters_count;
  REQUIRE(converter.Parse("123   ", &processed_characters_count) ==
          StringToIntegerResult(false, 123));
  REQUIRE(processed_characters_count == 6);
}

TEST_CASE("StringToIntegerConverterImpl AllowTrailingJunk", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(
      StringToNumberFlags::kAllowTrailingJunk, 0);
  Index processed_characters_count;
  REQUIRE(converter.Parse("123 12", &processed_characters_count) ==
          StringToIntegerResult(false, 123));
  REQUIRE(processed_characters_count == 3);
}

TEST_CASE("StringToIntegerConverterImpl AllowLeadingZeroForInteger",
          "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(
      StringToNumberFlags::kAllowLeadingZeroForInteger, 0);
  Index processed_characters_count;
  REQUIRE(converter.Parse("0x0012", &processed_characters_count) ==
          StringToIntegerResult(false, 0x12));
  REQUIRE(processed_characters_count == 6);

  REQUIRE(converter.Parse("000011", &processed_characters_count) ==
          StringToIntegerResult(false, 000011));
  REQUIRE(processed_characters_count == 6);

  REQUIRE(converter.Parse("0b0011", &processed_characters_count) ==
          StringToIntegerResult(false, 0b0011));
  REQUIRE(processed_characters_count == 6);
}

TEST_CASE("StringToIntegerConverterImpl CompositeFlags", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter(
      StringToNumberFlags::kAllowLeadingSpaces |
          StringToNumberFlags::kAllowTrailingJunk |
          StringToNumberFlags::kAllowLeadingZeroForInteger,
      0);
  Index processed_characters_count;

  REQUIRE(converter.Parse("   0x00123!!!", &processed_characters_count) ==
          StringToIntegerResult(false, 0x00123));
  REQUIRE(processed_characters_count == 10);
}

TEST_CASE("StringToIntegerConverterImpl OtherBase", "[string]") {
  using namespace cru;
  StringToIntegerConverter converter({}, 7);
  Index processed_characters_count;

  REQUIRE(converter.Parse("12", &processed_characters_count) ==
          StringToIntegerResult(false, 9));
  REQUIRE(processed_characters_count == 2);

  REQUIRE(converter.Parse("-12", &processed_characters_count) ==
          StringToIntegerResult(true, 9));
  REQUIRE(processed_characters_count == 3);

  converter.base = 11;
  REQUIRE(converter.Parse("1a", &processed_characters_count) ==
          StringToIntegerResult(false, 21));
  REQUIRE(processed_characters_count == 2);
}
