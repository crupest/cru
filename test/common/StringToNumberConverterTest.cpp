#include "cru/common/Exception.h"
#include "cru/common/StringToNumberConverter.h"

#include <gtest/gtest.h>

TEST(StringToIntegerConverterImpl, Base0) {
  using namespace cru;
  StringToIntegerConverterImpl converter(0, 0);
  Index processed_characters_count;

  ASSERT_EQ(converter.Parse("12345678", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 12345678));
  ASSERT_EQ(processed_characters_count, 8);

  ASSERT_EQ(converter.Parse("0", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 1);

  ASSERT_EQ(converter.Parse("012", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 012));
  ASSERT_EQ(processed_characters_count, 3);

  ASSERT_EQ(converter.Parse("0x12", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0x12));
  ASSERT_EQ(processed_characters_count, 4);

  ASSERT_EQ(converter.Parse("0X12", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0x12));
  ASSERT_EQ(processed_characters_count, 4);

  ASSERT_EQ(converter.Parse("0b101", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0b101));
  ASSERT_EQ(processed_characters_count, 5);

  ASSERT_EQ(converter.Parse("0B101", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0b101));
  ASSERT_EQ(processed_characters_count, 5);

  ASSERT_EQ(converter.Parse("-123", &processed_characters_count),
            StringToIntegerConverterImplResult(true, 123));
  ASSERT_EQ(processed_characters_count, 4);

    ASSERT_EQ(converter.Parse("-0x10", &processed_characters_count),
            StringToIntegerConverterImplResult(true, 0x10));
  ASSERT_EQ(processed_characters_count, 5);
}

TEST(StringToIntegerConverterImpl, Base0ForError) {
  using namespace cru;
  StringToIntegerConverterImpl converter(0, 0);
  Index processed_characters_count;

  ASSERT_EQ(converter.Parse("a", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);

  ASSERT_EQ(converter.Parse("0a", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);

  ASSERT_EQ(converter.Parse("0x", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);

  ASSERT_EQ(converter.Parse("0xx", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);

  ASSERT_EQ(converter.Parse(" 0", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);

  ASSERT_EQ(converter.Parse("0 ", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0));
  ASSERT_EQ(processed_characters_count, 0);
}

TEST(StringToIntegerConverterImpl, ThrowOnErrorFlag) {
  using namespace cru;
  StringToIntegerConverterImpl converter(StringToNumberFlags::kThrowOnError, 0);
  Index processed_characters_count;
  ASSERT_THROW(converter.Parse("?", &processed_characters_count), Exception);
}

TEST(StringToIntegerConverterImpl, AllowLeadingZeroFlag) {
  using namespace cru;
  StringToIntegerConverterImpl converter(
      StringToNumberFlags::kAllowLeadingSpaces, 0);
  Index processed_characters_count;
  ASSERT_EQ(converter.Parse("   123", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 123));
  ASSERT_EQ(processed_characters_count, 6);
}

TEST(StringToIntegerConverterImpl, AllowTrailingZeroFlag) {
  using namespace cru;
  StringToIntegerConverterImpl converter(
      StringToNumberFlags::kAllowTrailingSpaces, 0);
  Index processed_characters_count;
  ASSERT_EQ(converter.Parse("123   ", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 123));
  ASSERT_EQ(processed_characters_count, 6);
}

TEST(StringToIntegerConverterImpl, AllowTrailingJunk) {
  using namespace cru;
  StringToIntegerConverterImpl converter(
      StringToNumberFlags::kAllowLeadingZeroForInteger, 0);
  Index processed_characters_count;
  ASSERT_EQ(converter.Parse("123 12", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 123));
  ASSERT_EQ(processed_characters_count, 3);
}

TEST(StringToIntegerConverterImpl, AllowLeadingZeroForInteger) {
  using namespace cru;
  StringToIntegerConverterImpl converter(
      StringToNumberFlags::kAllowLeadingZeroForInteger, 0);
  Index processed_characters_count;
  ASSERT_EQ(converter.Parse("0x0012", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0x12));
  ASSERT_EQ(processed_characters_count, 6);

  ASSERT_EQ(converter.Parse("000011", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 000011));
  ASSERT_EQ(processed_characters_count, 6);

  ASSERT_EQ(converter.Parse("0b0011", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0b0011));
  ASSERT_EQ(processed_characters_count, 6);
}

TEST(StringToIntegerConverterImpl, CompositeFlags) {
  using namespace cru;
  StringToIntegerConverterImpl converter(
      StringToNumberFlags::kAllowLeadingSpaces |
          StringToNumberFlags::kAllowTrailingJunk |
          StringToNumberFlags::kAllowLeadingZeroForInteger,
      0);
  Index processed_characters_count;

  ASSERT_EQ(converter.Parse("   0x00123!!!", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 0x00123));
  ASSERT_EQ(processed_characters_count, 10);
}

TEST(StringToIntegerConverterImpl, OtherBase) {
  using namespace cru;
  StringToIntegerConverterImpl converter(0, 7);
  Index processed_characters_count;

  ASSERT_EQ(converter.Parse("12", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 9));
  ASSERT_EQ(processed_characters_count, 2);

  ASSERT_EQ(converter.Parse("-12", &processed_characters_count),
            StringToIntegerConverterImplResult(true, 9));
  ASSERT_EQ(processed_characters_count, 3);

  converter.base = 11;
  ASSERT_EQ(converter.Parse("1a", &processed_characters_count),
            StringToIntegerConverterImplResult(false, 21));
  ASSERT_EQ(processed_characters_count, 2);
}
