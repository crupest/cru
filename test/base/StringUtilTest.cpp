#include "cru/base/StringUtil.h"

#include <catch2/catch_test_macros.hpp>

using cru::Index;
using namespace cru::string;

TEST_CASE("StringUtil Split", "[string]") {
  REQUIRE(Split("abc", "b") == std::vector<std::string>{"a", "c"});
  REQUIRE(Split("abcd", "bc") == std::vector<std::string>{"a", "d"});
  REQUIRE(Split("abcdbcd", "bc") == std::vector<std::string>{"a", "d", "d"});
  REQUIRE(Split("aaa", "a") == std::vector<std::string>{"", "", "", ""});
}

TEST_CASE("StringUtil Utf8ByteType", "[string]") {
  REQUIRE(IsUtf8LeadingByte(0b00100000));
  REQUIRE(IsUtf8LeadingByte(0b01000000));
  REQUIRE(IsUtf8LeadingByte(0b11000000));
  REQUIRE(IsUtf8LeadingByte(0b11100000));
  REQUIRE(IsUtf8LeadingByte(0b11110000));
  REQUIRE(!IsUtf8LeadingByte(0b10100000));
  REQUIRE(!IsUtf8FollowingByte(0b00100000));
  REQUIRE(!IsUtf8FollowingByte(0b01000000));
  REQUIRE(!IsUtf8FollowingByte(0b11000000));
  REQUIRE(!IsUtf8FollowingByte(0b11100000));
  REQUIRE(!IsUtf8FollowingByte(0b11110000));
  REQUIRE(IsUtf8FollowingByte(0b10100000));
}

TEST_CASE("StringUtil Utf8NextCodePoint", "[string]") {
  std::string_view text = "aπ你🤣!";
  Index current = 0;
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == static_cast<Index>(text.size()));
}

TEST_CASE("StringUtil Utf8PreviousCodePoint", "[string]") {
  std::string_view text = "aπ你🤣!";
  Index current = text.size();
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf8PreviousCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == 0);
}

TEST_CASE("StringUtil Utf16NextCodePoint", "[string]") {
  std::u16string_view text = u"aπ你🤣!";
  Index current = 0;
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == static_cast<Index>(text.size()));
}

TEST_CASE("StringUtil Utf16PreviousCodePoint", "[string]") {
  std::u16string_view text = u"aπ你🤣!";
  Index current = text.size();
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == 0);
}

TEST_CASE("StringUtil Utf8CodePointIterator", "[string]") {
  std::string_view text = "aπ你🤣!";
  std::vector<CodePoint> code_points;

  for (auto cp : Utf8CodePointIterator(text.data(), text.size())) {
    code_points.push_back(cp);
  }

  std::vector<CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60, 0x1F923,
                                              0x0021};

  REQUIRE(code_points == expected_code_points);
}

TEST_CASE("StringUtil Utf16CodePointIterator", "[string]") {
  std::u16string_view text = u"aπ你🤣!";
  std::vector<CodePoint> code_points;

  for (auto cp : Utf16CodePointIterator(text.data(), text.size())) {
    code_points.push_back(cp);
  }

  std::vector<CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60, 0x1F923,
                                              0x0021};

  REQUIRE(code_points == expected_code_points);
}

TEST_CASE("ParseToNumber Work", "[string]") {
  auto r1 = ParseToNumber<int>("123");
  REQUIRE(r1.valid);
  REQUIRE(r1.value == 123);
  REQUIRE(r1.processed_char_count == 3);

  auto r2 = ParseToNumber<int>("123.123");
  REQUIRE(!r2.valid);

  auto r3 = ParseToNumber<float>("123.123");
  REQUIRE(r3.valid);
  REQUIRE(r3.value == 123.123f);
  REQUIRE(r3.processed_char_count == 7);

  auto r4 = ParseToNumber<float>("a123");
  REQUIRE(!r4.valid);
}

TEST_CASE("ParseToNumber AllowLeadingZeroFlag", "[string]") {
  auto r1 = ParseToNumber<int>("  123");
  REQUIRE(!r1.valid);

  auto r2 = ParseToNumber<int>("  123", ParseToNumberFlags::AllowLeadingSpaces);
  REQUIRE(r2.valid);
  REQUIRE(r2.value == 123);
  REQUIRE(r2.processed_char_count == 5);

  auto r3 = ParseToNumber<float>("  123.123");
  REQUIRE(!r3.valid);

  auto r4 =
      ParseToNumber<float>("  123.123", ParseToNumberFlags::AllowLeadingSpaces);
  REQUIRE(r4.valid);
  REQUIRE(r4.value == 123.123f);
  REQUIRE(r4.processed_char_count == 9);
}

TEST_CASE("StringToIntegerConverterImpl AllowTrailingSpacesFlag", "[string]") {
  auto r1 = ParseToNumber<int>("123  ");
  REQUIRE(!r1.valid);

  auto r2 =
      ParseToNumber<int>("123  ", ParseToNumberFlags::AllowTrailingSpaces);
  REQUIRE(r2.valid);
  REQUIRE(r2.value == 123);
  REQUIRE(r2.processed_char_count == 3);

  auto r3 = ParseToNumber<float>("123.123  ");
  REQUIRE(!r3.valid);

  auto r4 = ParseToNumber<float>("123.123  ",
                                 ParseToNumberFlags::AllowTrailingSpaces);
  REQUIRE(r4.valid);
  REQUIRE(r4.value == 123.123f);
  REQUIRE(r4.processed_char_count == 7);
}

TEST_CASE("StringToIntegerConverterImpl AllowTrailingJunk", "[string]") {
  auto r1 = ParseToNumber<int>("123ab");
  REQUIRE(!r1.valid);

  auto r2 = ParseToNumber<int>("123ab", ParseToNumberFlags::AllowTrailingJunk);
  REQUIRE(r2.valid);
  REQUIRE(r2.value == 123);
  REQUIRE(r2.processed_char_count == 3);

  auto r3 = ParseToNumber<float>("123.123ab");
  REQUIRE(!r3.valid);

  auto r4 =
      ParseToNumber<float>("123.123ab", ParseToNumberFlags::AllowTrailingJunk);
  REQUIRE(r4.valid);
  REQUIRE(r4.value == 123.123f);
  REQUIRE(r4.processed_char_count == 7);
}

TEST_CASE("StringToIntegerConverterImpl CompositeFlags", "[string]") {
  auto r1 =
      ParseToNumber<int>("  123ab", ParseToNumberFlags::AllowLeadingSpaces |
                                        ParseToNumberFlags::AllowTrailingJunk);
  REQUIRE(r1.valid);
  REQUIRE(r1.value == 123);
  REQUIRE(r1.processed_char_count == 5);

  auto r2 = ParseToNumber<float>("  123.123ab",
                                 ParseToNumberFlags::AllowLeadingSpaces |
                                     ParseToNumberFlags::AllowTrailingJunk);
  REQUIRE(r2.valid);
  REQUIRE(r2.value == 123.123f);
  REQUIRE(r2.processed_char_count == 9);
}

TEST_CASE("String ParseToNumberList", "[string]") {
  auto r1 = ParseToNumberList<int>("123 456 789");
  REQUIRE(r1 == std::vector<int>{123, 456, 789});

  auto r2 = ParseToNumberList<float>("1.1 2.2 3.3");
  REQUIRE(r2 == std::vector<float>{1.1f, 2.2f, 3.3f});
}

// TEST(WinString, IndexUtf8ToUtf16) {
//   using cru::platform::win::IndexUtf8ToUtf16;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 0, utf16_string), 0);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 1, utf16_string), 1);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 3, utf16_string), 2);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 6, utf16_string), 3);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 10, utf16_string), 5);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 11, utf16_string), 6);
// }

// TEST(WinString, IndexUtf16ToUtf8) {
//   using cru::platform::win::IndexUtf16ToUtf8;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 0, utf8_string), 0);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 1, utf8_string), 1);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 2, utf8_string), 3);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 3, utf8_string), 6);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 5, utf8_string), 10);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 6, utf8_string), 11);
// }
